/*
 * eval.c -- AST-walker
 *
 * Copyright (c) 2012 Nick Reynolds
 *  
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "eval.h"
#include "nodes.h"
#include <math.h>

JSValue * 
fh_eval(JSValue *ctx, Node *node)
{
  JSValue *result = JSUNDEF();
  switch(node->type) {
    case NODE_BOOL: return JSBOOL(node->val);
    case NODE_STR: return JSSTR(node->sval);
    case NODE_NUM: return JSNUM(node->val);
    case NODE_NULL: return JSNULL();
    case NODE_FUNC: return JSFUNC(node);
    case NODE_OBJ: return fh_obj(ctx, node);
    case NODE_ARR: return fh_arr(ctx, node);
    case NODE_CALL: return fh_call(ctx, node);
    case NODE_MEMBER: return fh_member(ctx, node);
    case NODE_IDENT: return fh_get_rec(ctx, node->sval);
    case NODE_BLOCK: return fh_eval(ctx, node->e1);
    case NODE_STMT_LST: return fh_stmt_lst(ctx, node);
    case NODE_SRC_LST: return fh_src_lst(ctx, node);
    case NODE_EXP_STMT: return fh_eval(ctx, node->e1);
    case NODE_EXP: return fh_exp(ctx, node);
    case NODE_IF: return fh_if(ctx, node);
    case NODE_TERN: return fh_if(ctx, node);
    case NODE_ASGN: return fh_assign(ctx, node);
    case NODE_RETURN: return fh_return(ctx, node);
    case NODE_BREAK: return fh_break();
    case NODE_PROP_LST: return fh_prop_lst(ctx, node);
    case NODE_PROP: fh_set(ctx, node->e1->sval, fh_eval(ctx, node->e2)); break;
    case NODE_WHILE: fh_while(ctx, node->e1, node->e2); break;
    case NODE_FOR: fh_for(ctx, node->e1, node->e2); break;
    case NODE_FORIN: fh_forin(ctx, node); break;
    case NODE_VAR_STMT: fh_var_stmt(ctx, node); break;
    case NODE_EMPT_STMT: break;
    default:
      fh_error(
        fh_new_state(node->line, node->column), E_SYNTAX,
        "Unsupported syntax type (%d)", node->type
      );
  }
  return result;
}

JSValue *
fh_exp(JSValue *ctx, Node *node)
{
  switch (node->sub_type) {
    case NODE_UNARY_POST: 
      return fh_eval_postfix_exp(ctx, node);
    case NODE_UNARY_PRE:
      return fh_eval_prefix_exp(ctx, node);
    default:
      return fh_eval_bin_exp(ctx, node);
  }
}

JSValue *
fh_if(JSValue *ctx, Node *node)
{
  if (JSCAST(fh_eval(ctx, node->e1), T_BOOLEAN)->boolean.val)
    return fh_eval(ctx, node->e2);
  else if (node->e3 != NULL)
    return fh_eval(ctx, node->e3);
  return JSUNDEF();
}

JSValue *
fh_return(JSValue *ctx, Node *node)
{
  JSValue *result = node->e1 ? fh_eval(ctx, node->e1) : JSUNDEF();
  if (result->type == T_FUNCTION) 
    result->function.closure = ctx;
  return result;
}

JSValue *
fh_break()
{
  JSValue *signal = JSNULL();
  signal->signal = S_BREAK;
  return signal;
}

void
fh_var_stmt(JSValue *ctx, Node *node)
{
  if (node->e2 != NULL)
    fh_set(ctx, node->e1->sval, fh_eval(ctx, node->e2));
  else
    fh_set(ctx, node->e1->sval, JSNULL());
}

JSValue *
fh_prop_lst(JSValue *ctx, Node *node)
{
  JSValue *result = JSUNDEF();
  rewind_node(node);
  while(!node->visited) result = fh_eval(ctx, pop_node(node));
  return result;
}

JSValue *
fh_stmt_lst(JSValue *ctx, Node *node)
{
  JSValue *result = NULL;
  Node *child;
  rewind_node(node);
  while(!node->visited) {
    child = pop_node(node);

    if (child->type == NODE_RETURN)
      return fh_return(ctx, child);
    if (child->type == NODE_BREAK)
      return fh_break();
    if (child->type == NODE_CONT)
      return JSUNDEF();

    result = fh_eval(ctx, child);
    if (result->signal == S_BREAK)
      return result;
  }
  return result ? result : JSUNDEF();
}

JSValue *
fh_src_lst(JSValue *ctx, Node *node)
{
  // First sweep for function declarations
  Node *child;
  while(!node->visited) {
    child = pop_node(node);
    if (child->type == NODE_FUNC) {
      fh_set(ctx, fh_str_from_node(ctx, child->e3)->string.ptr, JSFUNC(child));
    }
  }
  rewind_node(node);

  return fh_stmt_lst(ctx, node);
}

JSValue *
fh_obj(JSValue *ctx, Node *node)
{
  JSValue *obj = JSOBJ();
  obj->object.parent = ctx;
  if (node->e1 != NULL) fh_eval(obj, node->e1);
  return obj;
}

JSValue *
fh_arr(JSValue *ctx, Node *node)
{
  JSValue *arr = JSARR();
  if (node->e1 != NULL) {
    int i = 0;
    JSValue *str;
    while(!node->e1->visited) {
      str = JSCAST(JSNUM(i), T_STRING);
      fh_set(arr, str->string.ptr, fh_eval(ctx, pop_node(node->e1)));
      i++;
    }
    fh_arr_set_len(arr, i);
  }
  return arr;
}

JSValue *
fh_member(JSValue *ctx, Node *member)
{
  JSValue *id1, *id2, *parent;

  // In `x.foo` we'll take 'foo' literally, in `x[foo]` we need to eval 'foo'.
  // This distinction is stored as 0/1 in the val slot.
  id1 = member->val ? 
    JSCAST(fh_eval(ctx, member->e1), T_STRING) :
    fh_str_from_node(ctx, member->e1);
  id2 = fh_str_from_node(ctx, member->e2);

  parent = member->e2->type == NODE_MEMBER ? 
    fh_member(ctx, member->e2) :
    fh_get(ctx, id2->string.ptr);
  return fh_get_proto(parent, id1->string.ptr);
}

JSValue *
fh_assign(JSValue *ctx, Node *node)
{
  JSValue *val = fh_eval(ctx, node->e2);
  fh_do_assign(ctx, node->e1->sval, val, node->sval);
  return val;
}

void 
fh_do_assign(JSValue *obj, char *name, JSValue *val, char *op)
{
  if (STREQ(op, "=")) return fh_set_rec(obj, name, val);

  // Handle other assignment operators
  JSValue *cur = fh_get_rec(obj, name);
  if (STREQ(op, "+=")) return fh_set_rec(obj, name, fh_add(cur, val));
  if (STREQ(op, "-=")) return fh_set_rec(obj, name, fh_sub(cur, val));
  if (STREQ(op, "*=")) return fh_set_rec(obj, name, fh_mul(cur, val));
  if (STREQ(op, "/=")) return fh_set_rec(obj, name, fh_div(cur, val));
}

// ----------------------------------------------------------------------------
// Iteration Constructs
// ----------------------------------------------------------------------------

void
fh_while(JSValue *ctx, Node *cnd, Node *stmt)
{
  JSValue *result;

  while(JSCAST(fh_eval(ctx, cnd), T_BOOLEAN)->boolean.val) {
    result = fh_eval(ctx, stmt);
    if (result->signal == S_BREAK) break;
  }
}

void
fh_for(JSValue *ctx, Node *exp_grp, Node *stmt)
{
  JSValue *result;

  if (exp_grp->e1) {
    fh_eval(ctx, exp_grp->e1);
  }

  while(JSCAST(exp_grp->e2 ? fh_eval(ctx, exp_grp->e2) : JSBOOL(1), T_BOOLEAN)->boolean.val) {
    result = fh_eval(ctx, stmt);
    if (result->signal == S_BREAK) break;
    if (exp_grp->e3)
      fh_eval(ctx, exp_grp->e3);
  }
}

void
fh_forin(JSValue *ctx, Node *node)
{
  JSProp *p;
  JSValue *result,
          *proto = fh_eval(ctx, node->e2),
          *name = fh_str_from_node(ctx, node->e1);

  // If variable declaration:
  if (node->e1->type == NODE_IDENT)
    fh_set(ctx, name->string.ptr, JSNULL());

  // Note that during the first iteration, the prototype is the object.
  while(proto != NULL) {
    OBJ_ITER(proto, p) {
      if (p->enumerable) {
        // Assign to name, possibly undeclared assignment.
        fh_set_rec(ctx, name->string.ptr, JSSTR(p->name));
        result = fh_eval(ctx, node->e3);
        if (result->signal == S_BREAK) break;
      }
    }
    proto = proto->proto;
  }
}


// ----------------------------------------------------------------------------
// Function Application
// ----------------------------------------------------------------------------

JSValue *
fh_call(JSValue *ctx, Node *call)
{
  JSValue *maybe_func = fh_eval(ctx, call->e1);
  State *state = fh_new_state(call->line, call->column);
  if (maybe_func->type != T_FUNCTION)
    fh_error(state, E_TYPE, "%s is not a function", fh_typeof(maybe_func));
  return fh_function_call(ctx, state, maybe_func, call->e2);
}

JSArgs *
fh_build_args(JSValue *ctx, Node *args_node)
{
  JSArgs *args = malloc(sizeof(JSArgs));
  if (args_node->e1 == NULL) return args;
  if (!args_node->visited) {
    args->arg = fh_eval(ctx, pop_node(args_node));
    if (!args_node->visited)
      args->next = fh_build_args(ctx, args_node);
  }
  rewind_node(args_node);
  return args;
}

JSValue *
fh_function_call(JSValue *ctx, State *state, JSValue *func, Node *args_node)
{
  JSArgs *args = fh_build_args(ctx, args_node);

  if (func->function.is_native) {
    // Native functions are C functions referenced by pointer.
    rewind_node(args_node);
    JSNativeFunction native = func->function.native;
    JSValue *instance = func->function.instance;
    return (*native)(instance, args, state);
  }

  rewind_node(func->function.node);
  JSValue *func_scope = fh_setup_func_env(ctx, func, args);
  return fh_eval(func_scope, func->function.node->e2);
}

JSValue *
fh_setup_func_env(JSValue *ctx, JSValue *func, JSArgs *args)
{
  JSValue *arguments = JSOBJ();
  Node *func_node = func->function.node;
  JSValue *scope = func->function.closure ? func->function.closure : JSOBJ();

  scope->object.parent = ctx;
  fh_set(scope, "arguments", arguments);
  if (func_node->sval != NULL)
    fh_set(scope, func_node->sval, func);

  // Setup the (array-like) arguments object.
  fh_set(arguments, "callee", func);
  int i = 0;
  bool first = true;
  JSArgs *tmp = args;
  while(first || args->next != NULL)
  {
    if (!first)
      args = args->next;
    if (args->arg != NULL) {
      JSValue *id = JSCAST(JSNUM(i), T_STRING);
      fh_set(arguments, id->string.ptr, args->arg);
      i++;
    }
    first = false;
  }
  args = tmp;
  fh_set(arguments, "length", JSNUM((double)i));

  // Setup params as locals, if any
  if (func_node->e1 != NULL) {
    Node *params = func_node->e1;
    rewind_node(params);
    while(!params->visited) {
      if (args->arg != NULL) {
        fh_set(scope, pop_node(params)->sval, args->arg);
        if (args->next != NULL) 
          args = args->next;
        else
          args->arg = NULL;
      }
      else {
        fh_set(scope, pop_node(params)->sval, JSUNDEF());
      }
    } 
  }

  return scope;
}


// ----------------------------------------------------------------------------
// Unary Expressions
// ----------------------------------------------------------------------------

JSValue *
fh_eval_postfix_exp(JSValue *ctx, Node *node)
{
  JSValue *old_val = JSCAST(fh_eval(ctx, node->e1), T_NUMBER);
  char *op = node->sval;
  if (STREQ(op, "++")) {
    fh_set(ctx, node->e1->sval, fh_add(old_val, JSNUM(1)));
    return old_val;
  }
  if (STREQ(op, "--")) {
    fh_set(ctx, node->e1->sval, fh_sub(old_val, JSNUM(1)));
    return old_val;
  }
  assert(0);
}

JSValue *
fh_eval_prefix_exp(JSValue *ctx, Node *node)
{
  char *op = node->sval;

  if (STREQ(op, "delete")) {
    // TODO: get a hold of the evaluation's parent object, if any
    // and unset the property on the object.
    return JSBOOL(1);
  }
  if (STREQ(op, "typeof"))
    return JSSTR(fh_typeof(fh_eval(ctx, node->e1)));
  if (STREQ(op, "void")) {
    fh_eval(ctx, node->e1);
    return JSUNDEF();
  }

  if (STREQ(op, "+"))
    return JSCAST(fh_eval(ctx, node->e1), T_NUMBER);
  if (STREQ(op, "!"))
    return JSBOOL(!JSCAST(fh_eval(ctx, node->e1), T_BOOLEAN)->boolean.val);
  if (STREQ(op, "-")) {
    JSValue *x = JSCAST(fh_eval(ctx, node->e1), T_NUMBER);
    if (x->number.is_inf) return JSNINF();
    if (x->number.is_nan) return JSNAN();
    return JSNUM(-1 * x->number.val);
  }

  // Increment and decrement.
  JSValue *old_val = JSCAST(fh_eval(ctx, node->e1), T_NUMBER);
  JSValue *new_val;
  if (STREQ(op, "++")) {
    new_val = fh_add(old_val, JSNUM(1));
    fh_set(ctx, node->e1->sval, new_val);
    return new_val;
  }
  if (STREQ(op, "--")) {
    new_val = fh_sub(old_val, JSNUM(1));
    fh_set(ctx, node->e1->sval, new_val);
    return new_val;
  }

  assert(0);
}


// ----------------------------------------------------------------------------
// Binary Expressions
// ----------------------------------------------------------------------------

JSValue *
fh_eval_bin_exp(JSValue *ctx, Node *node)
{
  char *op = node->sval;

  // Logical (must short-circuit)
  if (STREQ(op, "&&")) return fh_and(ctx, node->e1, node->e2);
  if (STREQ(op, "||")) return fh_or(ctx, node->e1, node->e2);

  // At this point, we can safely evaluate both expressions.
  JSValue *a = fh_eval(ctx, node->e1);
  JSValue *b = fh_eval(ctx, node->e2);

  // Arithmetic and string operations
  if (STREQ(op, "+")) return fh_add(a, b);
  if (STREQ(op, "-")) return fh_sub(a, b);
  if (STREQ(op, "*")) return fh_mul(a, b); 
  if (STREQ(op, "/")) return fh_div(a, b);
  if (STREQ(op, "%")) return fh_mod(a, b);

  // (In)equality
  if (STREQ(op, "==")) return fh_eq(a, b, false);
  if (STREQ(op, "!=")) return fh_neq(a, b, false);
  if (STREQ(op, "===")) return fh_eq(a, b, true);
  if (STREQ(op, "!==")) return fh_neq(a, b, true);

  // Relational 
  if (STREQ(op, "<")) return fh_lt(a, b);
  if (STREQ(op, ">")) return fh_gt(a, b);
  if (STREQ(op, "<=")) 
    return JSBOOL(fh_lt(a, b)->boolean.val || fh_eq(a, b, false)->boolean.val);
  if (STREQ(op, ">="))
    return JSBOOL(fh_gt(a, b)->boolean.val || fh_eq(a, b, false)->boolean.val);

  assert(0);
}

JSValue *
fh_add(JSValue *a, JSValue *b)
{
  // Two strings => String
  if (T_BOTH(a, b, T_STRING))
    return JSSTR(fh_str_concat(a->string.ptr, b->string.ptr));

  // Two numbers => Number
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    return JSNUM(a->number.val + b->number.val);
  }

  // String and (null|undefined|Boolean|Number) => String
  if (T_XOR(a, b, T_NULL, T_STRING) || 
      T_XOR(a, b, T_BOOLEAN, T_STRING) ||
      T_XOR(a, b, T_NUMBER, T_STRING) ||
      T_XOR(a, b, T_UNDEF, T_STRING))
    return fh_add(JSCAST(a, T_STRING), JSCAST(b, T_STRING));

  // Number and undefined => NaN
  if (T_XOR(a, b, T_NUMBER, T_UNDEF)) 
    return JSNAN();

  // Number and null => Null
  if (T_XOR(a, b, T_NULL, T_NUMBER))
    return fh_add(JSCAST(a, T_NUMBER), JSCAST(b, T_NUMBER));

  // Number and Boolean => Number
  if (T_XOR(a, b, T_NUMBER, T_BOOLEAN))
    return fh_add(JSCAST(a, T_NUMBER), JSCAST(b, T_NUMBER));

  assert(0);
}

JSValue *
fh_sub(JSValue *a, JSValue *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    // Subtraction on NaN or Infinity results in NaN
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    return JSNUM(a->number.val - b->number.val);
  }
  return fh_sub(JSCAST(a, T_NUMBER), JSCAST(b, T_NUMBER));
}

JSValue *
fh_mul(JSValue *a, JSValue *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    return JSNUM(a->number.val * b->number.val);
  }
  return fh_mul(JSCAST(a, T_NUMBER), JSCAST(b, T_NUMBER));
}

JSValue *
fh_div(JSValue *a, JSValue *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    // Division by 0 yields Infinity
    if (b->number.val == 0) return JSINF();
    return JSNUM(a->number.val / b->number.val);
  }
  return fh_div(JSCAST(a, T_NUMBER), JSCAST(b, T_NUMBER));
}

JSValue *
fh_mod(JSValue *a, JSValue *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    return JSNUM(fmod(a->number.val, b->number.val));
  }
  return fh_mod(JSCAST(a, T_NUMBER), JSCAST(b, T_NUMBER));
}

JSValue *
fh_eq(JSValue *a, JSValue *b, bool strict)
{
  // Strict equality on different types is always false
  if (a->type != b->type && strict) return JSBOOL(0);

  // false != null
  if (T_XOR(a, b, T_NULL, T_BOOLEAN)) return JSBOOL(0);
  if (T_XOR(a, b, T_UNDEF, T_NULL)) return JSBOOL(1);
  if (T_BOTH(a, b, T_NULL) || T_BOTH(a, b, T_UNDEF)) return JSBOOL(1);
  if (T_BOTH(a, b, T_STRING))
    return JSBOOL(STREQ(a->string.ptr, b->string.ptr));
  if (T_BOTH(a, b, T_NUMBER)) {
    // Nothing equals NaN
    if (a->number.is_nan || b->number.is_nan) return JSBOOL(0);
    if (a->number.is_inf || b->number.is_inf) {
      if (a->number.is_inf && b->number.is_inf && 
          a->number.is_neg == b->number.is_neg) 
        return JSBOOL(1);
      return JSBOOL(0);
    }
    return JSBOOL(a->number.val == b->number.val);
  }
  // Objects are equal if they occupy the same memory address
  if (T_BOTH(a, b, T_OBJECT))
    return JSBOOL(a == b);

  return fh_eq(JSCAST(a, T_NUMBER), JSCAST(b, T_NUMBER), false);
}

JSValue *
fh_neq(JSValue *a, JSValue *b, bool strict)
{
  // Invert the result of fh_eq
  return JSBOOL(!fh_eq(a, b, strict)->boolean.val);
}

JSValue *
fh_gt(JSValue *a, JSValue *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    // _ > NaN == false, NaN > _ == false
    if (a->number.is_nan || b->number.is_nan) return JSBOOL(0);
    if (a->number.is_inf) return JSBOOL(1);
    if (b->number.is_inf) return JSBOOL(0);
    return JSBOOL(a->number.val > b->number.val);
  }
  if (T_BOTH(a, b, T_STRING)) 
    return JSBOOL(strcmp(a->string.ptr, b->string.ptr) > 0);

  if (a->type == T_UNDEF|| b->type == T_UNDEF) return JSBOOL(0);

  assert(0);
}

JSValue *
fh_lt(JSValue *a, JSValue *b)
{
  // !(a > b || a == b)
  return JSBOOL(!(fh_gt(a, b)->boolean.val || fh_eq(a, b, false)->boolean.val));
}

JSValue *
fh_and(JSValue *ctx, Node *a, Node *b)
{
  // && operator returns the first false value, or the second true value. 
  JSValue *aval = fh_eval(ctx, a);
  if (JSCAST(aval, T_BOOLEAN)->boolean.val) return fh_eval(ctx, b);
  return aval;
}

JSValue *
fh_or(JSValue *ctx, Node *a, Node *b)
{
  // || returns the first true value, or the second false value.
  JSValue *aval = fh_eval(ctx, a);
  if (JSCAST(aval, T_BOOLEAN)->boolean.val) return aval;
  return fh_eval(ctx, b);
}


// ----------------------------------------------------------------------------
// Utilities
// ----------------------------------------------------------------------------

JSValue *
fh_str_from_node(JSValue *ctx, Node *node)
{
  // Need to evaluate the node to a string key.
  // e.g. obj['a']  obj.a  arr[1]  arr[arr.length - 1]

  if (node->type == NODE_IDENT)
    return JSSTR(node->sval);
  return JSCAST(fh_eval(ctx, node), T_STRING);
}
