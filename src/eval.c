/*
 * eval.c -- AST-walking interpreter
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

#include <math.h>

#include "eval.h"
#include "flathead.h"
#include "props.h"
#include "nodes.h"
#include "str.h"
#include "gc.h"

JSValue * 
fh_eval(JSValue *ctx, Node *node)
{
  JSValue *result = JSUNDEF();
  switch(node->type) {
    case NODE_BOOL: return JSBOOL(node->val);
    case NODE_STR: return JSSTR(node->sval);
    case NODE_REGEXP: return JSREGEXP(node->sval);
    case NODE_NUM: return JSNUM(node->val);
    case NODE_NULL: return JSNULL();
    case NODE_FUNC: return fh_func(ctx, node);
    case NODE_OBJ: return fh_obj(ctx, node);
    case NODE_ARR: return fh_arr(ctx, node);
    case NODE_CALL: return fh_call(ctx, node);
    case NODE_NEW: return fh_new_exp(ctx, node->e1);
    case NODE_MEMBER: return fh_member(ctx, node);
    case NODE_IDENT: return fh_get_rec(ctx, node->sval);
    case NODE_BLOCK: return fh_eval(ctx, node->e1);
    case NODE_STMT_LST: return fh_stmt_lst(ctx, node);
    case NODE_SRC_LST: return fh_src_lst(ctx, node);
    case NODE_EXP_STMT: return fh_eval(ctx, node->e1);
    case NODE_EXP: return fh_exp(ctx, node);
    case NODE_IF: return fh_if(ctx, node);
    case NODE_TERN: return fh_if(ctx, node);
    case NODE_SWITCH_STMT: return fh_switch(ctx, node);
    case NODE_ASGN: return fh_assign(ctx, node);
    case NODE_RETURN: return fh_return(ctx, node);
    case NODE_VAR_STMT: return fh_eval(ctx, node->e1);
    case NODE_VAR_DEC_LST: return fh_eval_each(ctx, node);
    case NODE_VAR_DEC: return fh_var_dec(ctx, node);
    case NODE_BREAK: return fh_break();
    case NODE_PROP_LST: return fh_eval_each(ctx, node);
    case NODE_PROP: fh_set(ctx, node->e1->sval, fh_eval(ctx, node->e2)); break;
    case NODE_WHILE: fh_while(ctx, node->e1, node->e2); break;
    case NODE_FOR: fh_for(ctx, node->e1, node->e2); break;
    case NODE_FORIN: fh_forin(ctx, node); break;
    case NODE_EMPT_STMT: break;
    default:
      fh_error(
        fh_new_state(node->line, node->column), E_SYNTAX,
        "Unsupported syntax type (%d)", node->type
      );
  }
  return result;
}


// ----------------------------------------------------------------------------
// Source & Statement Lists
// ----------------------------------------------------------------------------

JSValue *
fh_src_lst(JSValue *ctx, Node *node)
{
  // First sweep for function declarations
  Node *child;
  while (!node->visited) {
    child = pop_node(node);
    if (child->type == NODE_FUNC) {
      fh_set(ctx, fh_str_from_node(ctx, child->e3)->string.ptr, JSFUNC(child));
    }
  }
  rewind_node(node);

  return fh_stmt_lst(ctx, node);
}

JSValue *
fh_stmt_lst(JSValue *ctx, Node *node)
{
  JSValue *result = NULL;
  Node *child;
  rewind_node(node);
  while (!node->visited) {
    child = pop_node(node);

    if (child->type == NODE_RETURN)
      return fh_return(ctx, child);
    if (child->type == NODE_BREAK)
      return fh_break();
    if (child->type == NODE_CONT)
      return JSUNDEF();

    // Break signals bubble up as special values with a signal flag.
    result = fh_eval(ctx, child);
    if (result->signal == S_BREAK)
      return result;
  }
  return result ? result : JSUNDEF();
}


// ----------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------

JSValue *
fh_new_exp(JSValue *ctx, Node *exp)
{
  JSValue *ctr;
  JSArgs *args;
  if (exp->e1->type == NODE_MEMBER) {
    ctr = fh_eval(ctx, exp->e1->e1);
    args = fh_build_args(ctx, exp->e1->e2);
  }
  else {
    ctr = fh_eval(ctx, exp);
    args = fh_new_args(0, 0, 0);
  }

  State *state = fh_new_state(exp->line, exp->column);
  state->construct = true;

  if (!IS_FUNC(ctr)) {
    fh_error(state, E_TYPE, "%s is not a function", fh_typeof(ctr));
  }

  JSValue *res,
          *obj = JSOBJ(),
          *proto = fh_get(ctr, "prototype");

  if (IS_OBJ(proto))
    obj->proto = proto;

  res = fh_function_call(ctx, obj, state, ctr, args); 
  return IS_OBJ(res) ? res : obj;
}


// ----------------------------------------------------------------------------
// Functions and Object & Array Literals
// ----------------------------------------------------------------------------

JSValue *
fh_func(JSValue *ctx, Node *node)
{
  JSValue *func = JSFUNC(node);
  // TODO: figure this out
  // func->function.closure = ctx;
  return func;
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
    while (!node->e1->visited) {
      fh_set(arr, JSNUMKEY(i++)->string.ptr, fh_eval(ctx, pop_node(node->e1)));
    }
    fh_arr_set_len(arr, i);
  }
  return arr;
}


// ----------------------------------------------------------------------------
// Membership
// ----------------------------------------------------------------------------

JSValue *
fh_member(JSValue *ctx, Node *member)
{
  JSValue *id1, *id2, *parent;

  // In `x.foo` we'll take 'foo' literally, in `x[foo]` we need to eval 'foo'.
  // This distinction is stored as 0/1 in the val slot.
  id1 = member->val ? 
    TO_STR(fh_eval(ctx, member->e1)) :
    fh_str_from_node(ctx, member->e1);
  id2 = fh_str_from_node(ctx, member->e2);

  parent = member->e2->type == NODE_MEMBER ? 
    fh_member(ctx, member->e2) :
    fh_get_rec(ctx, id2->string.ptr);

  // Special string type considerations
  if (parent->type == T_STRING) {

    // Handle array-like string character access.
    if (member->e1->type == NODE_NUM) {
      int i = member->e1->val, len = parent->string.length;
      if (i < len && i >= 0) {
        char *str = malloc(2);
        sprintf(str, "%c", parent->string.ptr[i]);
        return JSSTR(str);
      }
      return JSUNDEF();
    }

    // Strings don't have a hashmap, so we cheat a bit with the 'length' prop.
    if (STREQ(id1->string.ptr, "length"))
      return JSNUM(parent->string.length);
  }

  return fh_get_proto(parent, id1->string.ptr);
}


// ----------------------------------------------------------------------------
// Declaration & Assignment
// ----------------------------------------------------------------------------

JSValue *
fh_assign(JSValue *ctx, Node *node)
{
  JSValue *val = fh_eval(ctx, node->e2);
  char *key = node->e1->sval;

  if (node->e1->type == NODE_MEMBER) {
    Node *member = node->e1;
    ctx = member->e2->type == NODE_MEMBER ?
      fh_member(ctx, member->e2) :
      fh_get(ctx, fh_str_from_node(ctx, member->e2)->string.ptr);
    if (IS_ARR(ctx) && member->e1->type == NODE_NUM) {
      int val = member->e1->val;
      if (val >= ctx->object.length)
        fh_arr_set_len(ctx, val + 1);
    }
    key = fh_str_from_node(ctx, member->e1)->string.ptr;
  }
    
  fh_do_assign(ctx, key, val, node->sval);
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

JSValue *
fh_var_dec(JSValue *ctx, Node *node)
{
  if (node->e2 != NULL)
    fh_set(ctx, node->e1->sval, fh_eval(ctx, node->e2));
  else
    fh_set(ctx, node->e1->sval, JSUNDEF());
  return JSUNDEF();
}


// ----------------------------------------------------------------------------
// Iteration Constructs
// ----------------------------------------------------------------------------

void
fh_while(JSValue *ctx, Node *cnd, Node *stmt)
{
  JSValue *result;

  while (TO_BOOL(fh_eval(ctx, cnd))->boolean.val) {
    result = fh_eval(ctx, stmt);
    if (result->signal == S_BREAK) break;
  }
}

void
fh_for(JSValue *ctx, Node *exp_grp, Node *stmt)
{
  JSValue *result;

  if (exp_grp->e1)
    fh_eval(ctx, exp_grp->e1);

  while (TO_BOOL(exp_grp->e2 ? fh_eval(ctx, exp_grp->e2) : JSBOOL(1))->boolean.val) {
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
  if (node->e1->type == NODE_VAR_DEC) {
    fh_eval(ctx, node->e1);
    name = JSSTR(node->e1->e1->sval);
  }

  // Note that during the first iteration, the prototype is the object.
  while (proto != NULL) {
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
// Control
// ----------------------------------------------------------------------------

JSValue *
fh_return(JSValue *ctx, Node *node)
{
  JSValue *result = node->e1 ? fh_eval(ctx, node->e1) : JSUNDEF();
  if (result->type == T_FUNCTION) 
    result->function.closure = ctx;
  result->signal = S_BREAK;
  return result;
}

JSValue *
fh_break()
{
  JSValue *signal = JSNULL();
  signal->signal = S_BREAK;
  return signal;
}

JSValue *
fh_if(JSValue *ctx, Node *node)
{
  if (TO_BOOL(fh_eval(ctx, node->e1))->boolean.val)
    return fh_eval(ctx, node->e2);
  else if (node->e3 != NULL)
    return fh_eval(ctx, node->e3);
  return JSUNDEF();
}

JSValue *
fh_switch(JSValue *ctx, Node *node)
{
  JSValue *val, *result, *test = fh_eval(ctx, node->e1);

  Node *current,
       *caseblock = node->e2,
       *clauses_a = caseblock->e1, 
       *defaultclause = caseblock->e2, 
       *clauses_b = caseblock->e3;

  bool matched = false;

  // TODO: DRY these up
  
  // Check case clauses before the default case
  if (clauses_a) {
    while (!clauses_a->visited) {
      current = pop_node(clauses_a);
      val = fh_eval(ctx, current->e1);
      // Cases fall-through to the next when breaks are omitted.
      if (matched || fh_eq(test, val, true)->boolean.val) {
        matched = true;
        if (!current->e2) continue;
        result = fh_eval(ctx, current->e2);
        if (result->signal == S_BREAK) {
          result->signal = S_NONE;
          return result;
        }
      }
    }
  }

  // Check case clauses after the default case
  if (clauses_b) {
    while (!clauses_b->visited) {
      current = pop_node(clauses_b);
      val = fh_eval(ctx, current->e1);
      if (matched || fh_eq(test, val, true)->boolean.val) {
        matched = true;
        if (!current->e2) continue;
        result = fh_eval(ctx, current->e2);
        if (result->signal == S_BREAK) {
          result->signal = S_NONE;
          return result;
        }
      }
    }
  }

  // Take the default case if it exists and we haven't matched 
  // anything yet.
  if (defaultclause && !matched) {
    result = fh_eval(ctx, defaultclause->e2);
    if (result->signal == S_BREAK)
      result->signal = S_NONE;
    return result;
  }

  return JSUNDEF();
}


// ----------------------------------------------------------------------------
// Function Application
// ----------------------------------------------------------------------------

JSValue *
fh_call(JSValue *ctx, Node *call)
{
  JSValue *maybe_func = fh_eval(ctx, call->e1);
  State *state = fh_new_state(call->line, call->column);
  state->ctx = ctx;
  if (maybe_func->type != T_FUNCTION)
    fh_error(state, E_TYPE, "%s is not a function", fh_typeof(maybe_func));
  JSArgs *args = fh_build_args(ctx, call->e2);

  // Check for a bound this (see Function#bind)
  JSValue *this = maybe_func->function.bound_this ? 
    maybe_func->function.bound_this : ctx;

  return fh_function_call(ctx, this, state, maybe_func, args);
}

JSArgs *
fh_build_args(JSValue *ctx, Node *args_node)
{
  JSArgs *args = malloc(sizeof(JSArgs));
  args->arg = NULL;
  args->next = NULL;
  if (args_node->e1 == NULL) return args;
  if (!args_node->visited) {
    args->arg = fh_eval(ctx, pop_node(args_node));
    args->next = args_node->visited ? NULL : fh_build_args(ctx, args_node);
  }
  rewind_node(args_node);
  return args;
}

JSValue *
fh_function_call(JSValue *ctx, JSValue *this, State *state, 
                 JSValue *func, JSArgs *args)
{
  if (IS_UNDEF(this) || IS_NULL(this)) 
    this = fh_global();

  state->ctx = ctx;
  state->this = this;

  if (func->function.is_native) {
    // Native functions are C functions referenced by pointer.
    JSNativeFunction native = func->function.native;
    return (*native)(func->function.instance, args, state);
  }

  rewind_node(func->function.node);
  JSValue *func_scope = fh_setup_func_env(ctx, this, func, args);
  return fh_eval(func_scope, func->function.node->e2);
}

JSValue *
fh_setup_func_env(JSValue *ctx, JSValue *this, JSValue *func, JSArgs *args)
{
  JSValue *arguments = JSOBJ();
  Node *func_node = func->function.node;
  JSValue *scope = func->function.closure ? func->function.closure : JSOBJ();

  scope->object.parent = ctx;

  fh_set(scope, "this", this);
  fh_set(scope, "arguments", arguments);

  // Add the function name as ref to itself (if it has a name)
  if (func_node->sval != NULL)
    fh_set(scope, func_node->sval, func);

  // Set up the (array-like) arguments object.
  fh_set(arguments, "callee", func);
  fh_set(arguments, "length", JSNUM(ARGLEN(args)));
  int i;
  for (i = 0; i < ARGLEN(args); i++)
    fh_set(arguments, JSNUMKEY(i)->string.ptr, ARG(args, i));

  // Set up params as locals (if any)
  if (func_node->e1 != NULL) {
    Node *params = func_node->e1;
    rewind_node(params);
    // Go through each param and match it by position with an arg.
    while (!params->visited) {
      if (args->arg) {
        fh_set(scope, pop_node(params)->sval, args->arg);
        if (args->next)
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
// Expressions
// ----------------------------------------------------------------------------

JSValue *
fh_exp(JSValue *ctx, Node *node)
{
  switch (node->sub_type) {
    case NODE_UNARY_POST: 
      return fh_postfix_exp(ctx, node);
    case NODE_UNARY_PRE:
      return fh_prefix_exp(ctx, node);
    default:
      return fh_bin_exp(ctx, node);
  }
}


// ----------------------------------------------------------------------------
// Unary Expressions
// ----------------------------------------------------------------------------

JSValue *
fh_postfix_exp(JSValue *ctx, Node *node)
{
  JSValue *old_val = TO_NUM(fh_eval(ctx, node->e1));
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
fh_prefix_exp(JSValue *ctx, Node *node)
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
    return TO_NUM(fh_eval(ctx, node->e1));
  if (STREQ(op, "!"))
    return JSBOOL(!TO_BOOL(fh_eval(ctx, node->e1))->boolean.val);
  if (STREQ(op, "-")) {
    JSValue *x = TO_NUM(fh_eval(ctx, node->e1));
    if (x->number.is_inf) return JSNINF();
    if (x->number.is_nan) return JSNAN();
    return JSNUM(-1 * x->number.val);
  }

  // Increment and decrement.
  JSValue *old_val = TO_NUM(fh_eval(ctx, node->e1));
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
fh_bin_exp(JSValue *ctx, Node *node)
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
  if (STREQ(op, "instanceof")) {
    if (b->type != T_FUNCTION)
      fh_error(NULL, E_TYPE, "Expecting a function in 'instanceof' check");
    return fh_has_instance(b, a);
  }
  if (STREQ(op, "in"))
    if (b->type != T_OBJECT)
      fh_error(NULL, E_TYPE, "Expecting an object with 'in' operator");
    return fh_has_property(b, TO_STR(a)->string.ptr);

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
    return fh_add(TO_STR(a), TO_STR(b));

  // Number and undefined => NaN
  if (T_XOR(a, b, T_NUMBER, T_UNDEF)) 
    return JSNAN();

  // Number and null => Null
  if (T_XOR(a, b, T_NULL, T_NUMBER))
    return fh_add(TO_NUM(a), TO_NUM(b));

  // Number and Boolean => Number
  if (T_XOR(a, b, T_NUMBER, T_BOOLEAN))
    return fh_add(TO_NUM(a), TO_NUM(b));

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
  return fh_sub(TO_NUM(a), TO_NUM(b));
}

JSValue *
fh_mul(JSValue *a, JSValue *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    return JSNUM(a->number.val * b->number.val);
  }
  return fh_mul(TO_NUM(a), TO_NUM(b));
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
  return fh_div(TO_NUM(a), TO_NUM(b));
}

JSValue *
fh_mod(JSValue *a, JSValue *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    return JSNUM(fmod(a->number.val, b->number.val));
  }
  return fh_mod(TO_NUM(a), TO_NUM(b));
}

JSValue *
fh_eq(JSValue *a, JSValue *b, bool strict)
{
  // Strict equality on different types is always false
  if (a->type != b->type && strict) return JSBOOL(0);

  // false != null
  if (T_XOR(a, b, T_NULL, T_BOOLEAN)) return JSBOOL(0);
  if (T_XOR(a, b, T_NULL, T_OBJECT)) return JSBOOL(0);
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
  if (T_BOTH(a, b, T_OBJECT) || T_BOTH(a, b, T_FUNCTION))
    return JSBOOL(a == b);

  return fh_eq(TO_NUM(a), TO_NUM(b), false);
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
  if (TO_BOOL(aval)->boolean.val) return fh_eval(ctx, b);
  return aval;
}

JSValue *
fh_or(JSValue *ctx, Node *a, Node *b)
{
  // || returns the first true value, or the second false value.
  JSValue *aval = fh_eval(ctx, a);
  if (TO_BOOL(aval)->boolean.val) return aval;
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
  return TO_STR(fh_eval(ctx, node));
}

JSValue *
fh_eval_each(JSValue *ctx, Node *node)
{
  JSValue *result = JSUNDEF();
  rewind_node(node);
  while (!node->visited) result = fh_eval(ctx, pop_node(node));
  return result;
}
