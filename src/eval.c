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

js_val * 
fh_eval(js_val *ctx, ast_node *node)
{
  if (!node) return JSUNDEF();

  switch(node->type) {
    case NODE_BOOL:        return JSBOOL(node->val);
    case NODE_STR:         return JSSTR(node->sval);
    case NODE_REGEXP:      return JSREGEXP(node->sval);
    case NODE_NUM:         return JSNUM(node->val);
    case NODE_NULL:        return JSNULL();
    case NODE_FUNC:        return JSFUNC(node);

    case NODE_OBJ:         return fh_obj(ctx, node);
    case NODE_ARR:         return fh_arr(ctx, node);
    case NODE_CALL:        return fh_call_exp(ctx, node);
    case NODE_NEW:         return fh_new_exp(ctx, node);
    case NODE_MEMBER:      return fh_member(ctx, node);
    case NODE_IDENT:       return fh_ident(ctx, node);
    case NODE_STMT_LST:    return fh_stmt_lst(ctx, node);
    case NODE_SRC_LST:     return fh_src_lst(ctx, node);
    case NODE_EXP:         return fh_exp(ctx, node);
    case NODE_SWITCH_STMT: return fh_switch(ctx, node);
    case NODE_ASGN:        return fh_assign(ctx, node);
    case NODE_RETURN:      return fh_return(ctx, node);
    case NODE_VAR_DEC:     return fh_var_dec(ctx, node, false);
    case NODE_BREAK:       return fh_break();
    case NODE_IF:          return fh_if(ctx, node);
    case NODE_TERN:        return fh_if(ctx, node);
    case NODE_BLOCK:       return fh_eval(ctx, node->e1);
    case NODE_EXP_STMT:    return fh_eval(ctx, node->e1);
    case NODE_VAR_STMT:    return fh_eval(ctx, node->e1);
    case NODE_VAR_DEC_LST: return fh_eval_each(ctx, node);
    case NODE_PROP_LST:    return fh_eval_each(ctx, node);

    case NODE_PROP:        fh_set(ctx, node->e1->sval, fh_eval(ctx, node->e2)); break;
    case NODE_WHILE:       fh_while(ctx, node->e1, node->e2); break;
    case NODE_FOR:         fh_for(ctx, node->e1, node->e2); break;
    case NODE_FORIN:       fh_forin(ctx, node); break;
    case NODE_EMPT_STMT:   break;

    default:
      fh_error(
        fh_new_state(node->line, node->column), E_SYNTAX,
        "Unsupported syntax type (%d)", node->type
      );
  }

  return JSUNDEF();
}


// ----------------------------------------------------------------------------
// Source & Statement Lists
// ----------------------------------------------------------------------------

js_val *
fh_src_lst(js_val *ctx, ast_node *node)
{
  fh_func_dec_scan(ctx, node);
  fh_var_dec_scan(ctx, node);

  return fh_stmt_lst(ctx, node);
}

void
fh_func_dec_scan(js_val *ctx, ast_node *node)
{
  // Sweep for function declarations
  ast_node *child;
  while (!node->visited) {
    child = pop_node(node);
    if (child->type == NODE_FUNC) {
      char *name = fh_str_from_node(ctx, child->e3)->string.ptr;
      fh_set_prop(ctx, name, JSFUNC(child), P_WRITE | P_ENUM);
    }
  }
  rewind_node(node);
}

void
fh_var_dec_scan(js_val *ctx, ast_node *node)
{
  // Sweep for variable declarations

  // Don't touch functions (stay within our current scope)
  if (node->type == NODE_FUNC) return;

  // Declare the variable and possibly convert it to an assignment
  if (node->type == NODE_VAR_DEC) {
    fh_var_dec(ctx, node, true); // true to ignore rhs
    if (node->e2) {
      node->type = NODE_ASGN;
      node->sval = "=";
    }
    else {
      // Signals that it has been hoisted
      node->val = 1; 
    }
  }

  // Recurse sub nodes, will hit the whole tree.
  if (node->e1) fh_var_dec_scan(ctx, node->e1);
  if (node->e2) fh_var_dec_scan(ctx, node->e2);
  if (node->e3) fh_var_dec_scan(ctx, node->e3);
}

js_val *
fh_stmt_lst(js_val *ctx, ast_node *node)
{
  js_val *result = NULL;
  ast_node *child;
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

js_val *
fh_new_exp(js_val *ctx, ast_node *exp)
{
  js_val *ctr;
  js_args *args;

  // new F(x, y, z)
  if (exp->e1 && exp->e1->type == NODE_MEMBER) {
    ctr = fh_eval(ctx, exp->e1->e2);
    args = fh_build_args(ctx, exp->e1->e1);
  }
  // new F
  else {
    ctr = fh_eval(ctx, exp->e1);
    args = fh_new_args(0, 0, 0);
  }

  eval_state *state= fh_new_state(exp->line, exp->column);
  state->construct = true;

  if (!IS_FUNC(ctr))
    fh_error(state, E_TYPE, "%s is not a function", fh_typeof(ctr));

  js_val *res, *obj = JSOBJ(), *proto = fh_get(ctr, "prototype");

  res = fh_call(ctx, obj, state, ctr, args); 
  res = IS_OBJ(res) ? res : obj;

  // Automatically set the prototype to use the constructor's "prototype"
  // property if valid and the result's proto member appears to be unmodified.
  if (IS_OBJ(proto) && res->proto == fh->object_proto)
    res->proto = IS_OBJ(proto) ? proto : fh->object_proto;

  return res;
}


// ----------------------------------------------------------------------------
// Object & Array Literals
// ----------------------------------------------------------------------------

js_val *
fh_obj(js_val *ctx, ast_node *node)
{
  js_val *obj = JSOBJ();
  obj->object.parent = ctx;
  if (node->e1 != NULL) fh_eval(obj, node->e1);
  return obj;
}

js_val *
fh_arr(js_val *ctx, ast_node *node)
{
  js_val *arr = JSARR();
  if (node->e1 != NULL) {
    int i = 0;
    while (!node->e1->visited) {
      fh_set(arr, JSNUMKEY(i++)->string.ptr, fh_eval(ctx, pop_node(node->e1)));
    }
    fh_set_len(arr, i);
  }
  return arr;
}


// ----------------------------------------------------------------------------
// Lookups & Member Expressions
// ----------------------------------------------------------------------------

js_val *
fh_ident(js_val *ctx, ast_node *id)
{
  js_prop *prop = fh_get_prop_rec(ctx, id->sval);
  if (!prop) {
    eval_state *state = fh_new_state(id->line, id->column);
    fh_error(state, E_REFERENCE, "%s is not defined", id->sval);
  }
  return prop->ptr;
}

js_val *
fh_member(js_val *ctx, ast_node *member)
{
  js_val *parent = fh_member_parent(ctx, member);
  js_val *child_name = fh_member_child(ctx, member);

  // Handle array-like string character access.
  if (IS_STR(parent) && member->e1->type == NODE_NUM) {
    int i = member->e1->val, len = parent->string.length;
    if (i < len && i >= 0) {
      char *str = malloc(sizeof(char) + 1);
      sprintf(str, "%c", parent->string.ptr[i]);
      return JSSTR(str);
    }
    return JSUNDEF();
  }

  return fh_get_proto(parent, child_name->string.ptr);
}

js_val *
fh_member_parent(js_val *ctx, ast_node *member)
{
  return member->e2->type == NODE_MEMBER ? 
    fh_member(ctx, member->e2) :
    fh_eval(ctx, member->e2);
}

js_val *
fh_member_child(js_val *ctx, ast_node *member)
{
  // In `x.foo` we'll take 'foo' literally, in `x[foo]` we need to eval 'foo'.
  // This distinction is stored as 0/1 in the val slot.
  return member->val ? 
    TO_STR(fh_eval(ctx, member->e1)) :
    fh_str_from_node(ctx, member->e1);
}


// ----------------------------------------------------------------------------
// Declaration & Assignment
// ----------------------------------------------------------------------------

js_val *
fh_assign(js_val *ctx, ast_node *node)
{
  js_val *val = fh_eval(ctx, node->e2);
  char *key = node->e1->sval;

  if (node->e1->type == NODE_MEMBER) {
    js_val *old_ctx = ctx; 

    ast_node *member = node->e1;
    ctx = member->e2->type == NODE_MEMBER ?
      fh_member(ctx, member->e2) :
      fh_get_rec(ctx, fh_str_from_node(ctx, member->e2)->string.ptr);

    key = fh_str_from_node(old_ctx, member->e1)->string.ptr;

    // Set the array length.
    if (IS_ARR(ctx)) {
      char *err;
      unsigned long val = strtod(key, &err);
      if (*err == 0 && val >= ctx->object.length)
          fh_set_len(ctx, val + 1);
    }
  }

  if (IS_OBJ(ctx))
    fh_do_assign(ctx, key, val, node->sval);
  return val;
}

void 
fh_do_assign(js_val *obj, char *name, js_val *val, char *op)
{
  if (STREQ(op, "=")) { 
    fh_set_rec(obj, name, val);
    return;
  }

  // Handle other assignment operators
  js_val *cur = fh_get_rec(obj, name);
  if (STREQ(op, "+=")) fh_set_rec(obj, name, fh_add(cur, val));
  if (STREQ(op, "-=")) fh_set_rec(obj, name, fh_sub(cur, val));
  if (STREQ(op, "*=")) fh_set_rec(obj, name, fh_mul(cur, val));
  if (STREQ(op, "/=")) fh_set_rec(obj, name, fh_div(cur, val));
}

js_val *
fh_var_dec(js_val *ctx, ast_node *node, bool ignore_rval)
{
  // If node->val is set, the variable declaration has already been hoisted and
  // should not be touched.
  if (!node->val) {
    if (node->e2 == NULL || ignore_rval)
      fh_set_prop(ctx, node->e1->sval, JSUNDEF(), P_WRITE | P_ENUM);
    else
      fh_set_prop(ctx, node->e1->sval, fh_eval(ctx, node->e2), P_WRITE | P_ENUM);
  }
  return JSUNDEF();
}


// ----------------------------------------------------------------------------
// Iteration Constructs
// ----------------------------------------------------------------------------

void
fh_while(js_val *ctx, ast_node *cnd, ast_node *stmt)
{
  js_val *result;

  while (TO_BOOL(fh_eval(ctx, cnd))->boolean.val) {
    result = fh_eval(ctx, stmt);
    if (result->signal == S_BREAK) break;
  }
}

void
fh_for(js_val *ctx, ast_node *exp_grp, ast_node *stmt)
{
  js_val *result;

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
fh_forin(js_val *ctx, ast_node *node)
{
  js_val *result, *obj, *env, *name;

  obj = fh_eval(ctx, node->e2);
  env = ctx;

  if (node->e1->type == NODE_MEMBER) {
    env = fh_member_parent(ctx, node->e1);
    name = fh_member_child(ctx, node->e1);
  }
  else 
    name = fh_str_from_node(ctx, node->e1);

  js_prop *p;
  while (obj != NULL) {
    OBJ_ITER(obj, p) {
      if (p->enumerable) {
        // Assign to name, possibly undeclared assignment.
        fh_set_rec(env, name->string.ptr, JSSTR(p->name));
        result = fh_eval(ctx, node->e3);
        if (result->signal == S_BREAK) break;
      }
    }
    obj = obj->proto;
  }
}


// ----------------------------------------------------------------------------
// Control
// ----------------------------------------------------------------------------

js_val *
fh_return(js_val *ctx, ast_node *node)
{
  js_val *result = node->e1 ? fh_eval(ctx, node->e1) : JSUNDEF();
  if (IS_FUNC(result))
    result->object.scope = ctx;
  result->signal = S_BREAK;
  return result;
}

js_val *
fh_break()
{
  js_val *signal = JSNULL();
  signal->signal = S_BREAK;
  return signal;
}

js_val *
fh_if(js_val *ctx, ast_node *node)
{
  if (TO_BOOL(fh_eval(ctx, node->e1))->boolean.val)
    return fh_eval(ctx, node->e2);
  else if (node->e3 != NULL)
    return fh_eval(ctx, node->e3);
  return JSUNDEF();
}

js_val *
fh_switch(js_val *ctx, ast_node *node)
{
  js_val *val, *result, *test = fh_eval(ctx, node->e1);

  ast_node *current,
       *caseblock = node->e2,
       *clauses_a = caseblock->e1, 
       *defaultclause = caseblock->e2, 
       *clauses_b = caseblock->e3;

  bool matched = false;

  // Check case clauses before and after the default case
  ast_node *clauses, *clauses_lst[] = {clauses_a, clauses_b};
  int i;
  for (i = 0; i < 2; i++) {
    clauses = clauses_lst[i];
    if (clauses) {
      while (!clauses->visited) {
        current = pop_node(clauses);
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

js_val *
fh_call_exp(js_val *ctx, ast_node *call)
{
  js_val *maybe_func = fh_eval(ctx, call->e1);
  eval_state *state= fh_new_state(call->line, call->column);
  state->ctx = ctx;
  if (!IS_FUNC(maybe_func))
    fh_error(state, E_TYPE, "%s is not a function", fh_typeof(maybe_func));
  js_args *args = fh_build_args(ctx, call->e2);

  // Check for a bound this (see Function#bind)
  js_val *this = maybe_func->object.bound_this ? 
    maybe_func->object.bound_this : fh_get(ctx, "this");

  return fh_call(ctx, this, state, maybe_func, args);
}

js_args *
fh_build_args(js_val *ctx, ast_node *args_node)
{
  js_args *args = malloc(sizeof(js_args));
  if (args == NULL)
    fh_error(NULL, E_ERROR, "allocation failed");
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

js_val *
fh_call(js_val *ctx, js_val *this, eval_state *state, js_val *func, js_args *args)
{
  if (IS_UNDEF(this) || IS_NULL(this)) 
    this = fh->global;

  state->ctx = ctx;
  state->this = this;

  if (func->object.native) {
    // Native functions are C functions referenced by pointer.
    js_native_function *native = func->object.nativefn;
    js_val *instance = func->object.instance;

    // new Number, new Boolean, etc. return wrapper objects. 
    // Here we resolve the wrapper to the value it wraps.
    if (instance && IS_OBJ(instance) && instance->object.primitive)
      instance = instance->object.primitive;

    return native(instance, args, state);
  }

  rewind_node(func->object.node);
  js_val *func_scope = fh_setup_call_env(ctx, this, func, args);
  return fh_eval(func_scope, func->object.node->e2);
}

js_val *
fh_setup_call_env(js_val *ctx, js_val *this, js_val *func, js_args *args)
{
  js_val *arguments = JSOBJ();
  ast_node *func_node = func->object.node;
  js_val *scope = func->object.scope ? func->object.scope : JSOBJ();

  scope->object.parent = ctx;

  fh_set(scope, "this", this);
  fh_set(scope, "arguments", arguments);

  // Add the function name as ref to itself (if it has a name)
  // TODO: Take another look at this. Under what circumstances is the name
  // set in the function environemnt?
  if (func_node->sval != NULL)
    fh_set(scope, func_node->sval, func);

  // Set up the (array-like) arguments object.
  int i, arglen = ARGLEN(args);
  for (i = 0; i < arglen; i++)
    fh_set(arguments, JSNUMKEY(i)->string.ptr, ARG(args, i));
  fh_set_class(arguments, "Arguments");
  fh_set(arguments, "callee", func);
  fh_set(arguments, "length", JSNUM(arglen));

  // Set up params as locals (if any)
  if (func_node->e1 != NULL) {
    ast_node *params = func_node->e1;
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

js_val *
fh_exp(js_val *ctx, ast_node *node)
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

js_val *
fh_postfix_exp(js_val *ctx, ast_node *node)
{
  js_val *old_val = TO_NUM(fh_eval(ctx, node->e1));
  char *op = node->sval;
  if (STREQ(op, "++")) {
    fh_set_rec(ctx, node->e1->sval, fh_add(old_val, JSNUM(1)));
    return old_val;
  }
  if (STREQ(op, "--")) {
    fh_set_rec(ctx, node->e1->sval, fh_sub(old_val, JSNUM(1)));
    return old_val;
  }
  UNREACHABLE();
}

js_val *
fh_prefix_exp(js_val *ctx, ast_node *node)
{
  char *op = node->sval;

  if (STREQ(op, "delete"))
    return fh_delete(ctx, node);
  if (STREQ(op, "typeof")) {
    if (node->e1->type == NODE_IDENT)
      return JSSTR(fh_typeof(fh_get_rec(ctx, node->e1->sval)));
    return JSSTR(fh_typeof(fh_eval(ctx, node->e1)));
  }
  if (STREQ(op, "void")) {
    fh_eval(ctx, node->e1);
    return JSUNDEF();
  }
  if (STREQ(op, "+"))
    return TO_NUM(fh_eval(ctx, node->e1));
  if (STREQ(op, "!"))
    return JSBOOL(!TO_BOOL(fh_eval(ctx, node->e1))->boolean.val);
  if (STREQ(op, "-")) {
    js_val *x = TO_NUM(fh_eval(ctx, node->e1));
    if (x->number.is_inf) return JSNINF();
    if (x->number.is_nan) return JSNAN();
    return JSNUM(-1 * x->number.val);
  }

  js_val *old_val = TO_NUM(fh_eval(ctx, node->e1));
  js_val *new_val;

  // Increment and decrement.
  if (STREQ(op, "++")) {
    new_val = fh_add(old_val, JSNUM(1));
    fh_set_rec(ctx, node->e1->sval, new_val);
    return new_val;
  }
  if (STREQ(op, "--")) {
    new_val = fh_sub(old_val, JSNUM(1));
    fh_set_rec(ctx, node->e1->sval, new_val);
    return new_val;
  }

  // Bitwise NOT
  if (STREQ(op, "~")) {
    old_val = fh_to_int32(old_val);
    int old_val_int32 = (int)old_val->number.val;
    return JSNUM(~old_val_int32);
  }

  UNREACHABLE();
}

js_val *
fh_delete(js_val *ctx, ast_node *node)
{
  js_val *name = fh_str_from_node(ctx, node->e1), 
         *env = ctx;

  if (node->e1->type == NODE_MEMBER) {
    env = fh_member_parent(ctx, node->e1);
    name = fh_member_child(ctx, node->e1);
  }
  else {
    name = fh_str_from_node(ctx, node->e1);
  }

  js_prop *prop = fh_get_prop(env, name->string.ptr);
  if (!prop->configurable)
    return JSBOOL(0);
  return JSBOOL(fh_del_prop(env, name->string.ptr));
}


// ----------------------------------------------------------------------------
// Binary Expressions
// ----------------------------------------------------------------------------

js_val *
fh_bin_exp(js_val *ctx, ast_node *node)
{
  char *op = node->sval;

  // Logical (must short-circuit)
  if (STREQ(op, "&&")) return fh_and(ctx, node->e1, node->e2);
  if (STREQ(op, "||")) return fh_or(ctx, node->e1, node->e2);

  // At this point, we can safely evaluate both expressions.
  js_val *a = fh_eval(ctx, node->e1);
  js_val *b = fh_eval(ctx, node->e2);

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
  if (STREQ(op, "<")) return fh_lt(a, b, false);
  if (STREQ(op, ">")) return fh_gt(a, b, false);
  if (STREQ(op, "<=")) return fh_lt(a, b, true);
  if (STREQ(op, ">=")) return fh_gt(a, b, true);
  if (STREQ(op, "instanceof")) {
    if (!IS_FUNC(b))
      fh_error(NULL, E_TYPE, "Expecting a function in 'instanceof' check");
    return fh_has_instance(b, a);
  }
  if (STREQ(op, "in")) {
    if (!IS_OBJ(b))
      fh_error(NULL, E_TYPE, "Expecting an object with 'in' operator");
    return fh_has_property(b, TO_STR(a)->string.ptr);
  }

  int a_int32 = fh_to_int32(a)->number.val;
  int b_int32 = fh_to_int32(b)->number.val;

  // Bitwise Logical
  if (STREQ(op, "&")) return JSNUM(a_int32 & b_int32);
  if (STREQ(op, "^")) return JSNUM(a_int32 ^ b_int32);
  if (STREQ(op, "|")) return JSNUM(a_int32 | b_int32);

  unsigned a_uint32 = fh_to_uint32(a)->number.val;
  unsigned b_uint32 = fh_to_uint32(b)->number.val;
  unsigned shift_cnt = b_uint32 & 0x1F;

  // Bitwise Shift
  if (STREQ(op, "<<")) return JSNUM(a_int32 << shift_cnt);
  if (STREQ(op, ">>")) return JSNUM(a_int32 >> shift_cnt);
  if (STREQ(op, ">>>")) return JSNUM(a_uint32 >> shift_cnt);

  UNREACHABLE();
}

js_val *
fh_add(js_val *a, js_val *b)
{
  a = fh_to_primitive(a, T_NUMBER);
  b = fh_to_primitive(b, T_NUMBER);

  if (IS_STR(a) || IS_STR(b))
    return JSSTR(fh_str_concat(TO_STR(a)->string.ptr, TO_STR(b)->string.ptr));

  if (!IS_NUM(a) || !IS_NUM(b))
    return fh_add(TO_NUM(a), TO_NUM(b));

  if (IS_NAN(a) || IS_NAN(b))
    return JSNAN();

  if (IS_INF(a) && IS_INF(b)) {
    if (a->number.is_neg != b->number.is_neg) return JSNAN();
    if (a->number.is_neg) return JSNINF();
    return JSINF();
  }

  if (IS_INF(a)) return a;
  if (IS_INF(b)) return b;

  return JSNUM(a->number.val + b->number.val);
}

js_val *
fh_sub(js_val *a, js_val *b)
{
  // a - b  <==>  a + (-b)
  return fh_add(TO_NUM(a), JSNUM(-1 * TO_NUM(b)->number.val));
}

js_val *
fh_mul(js_val *a, js_val *b)
{
  a = TO_NUM(a), b = TO_NUM(b);

  if (IS_NAN(a) || IS_NAN(b)) return JSNAN();
  if (IS_INF(a) || IS_INF(b)) {
    if (IS_INF(a) && IS_INF(b))
      return a->number.is_neg == b->number.is_neg ? JSINF() : JSNINF();

    js_val *fin = IS_INF(a) ? b : a;
    js_val *inf = IS_INF(a) ? a : b;

    if (fin->number.val == 0) return JSNAN();
    return (fin->number.val < 0) == inf->number.is_neg ? JSINF() : JSNINF();
  }

  return JSNUM(a->number.val * b->number.val);
}

js_val *
fh_div(js_val *a, js_val *b)
{
  a = TO_NUM(a), b = TO_NUM(b);

  if (IS_NAN(a) || IS_NAN(b)) return JSNAN();
  if (IS_INF(a) && IS_INF(b)) return JSNAN();
  if (IS_INF(a))
    return a->number.is_neg ? JSNINF() : JSINF();

  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    // Division by 0 yields Infinity
    if (b->number.val == 0) return JSINF();
    return JSNUM(a->number.val / b->number.val);
  }
  return fh_div(TO_NUM(a), TO_NUM(b));
}

js_val *
fh_mod(js_val *a, js_val *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    return JSNUM(fmod(a->number.val, b->number.val));
  }
  return fh_mod(TO_NUM(a), TO_NUM(b));
}

js_val *
fh_eq(js_val *a, js_val *b, bool strict)
{
  // Strict equality on different types is always false
  if (a->type != b->type && strict) return JSBOOL(0);

  // Same type
  if (a->type == b->type) {
    if (IS_UNDEF(a) || IS_NULL(a)) return JSBOOL(1);
    if (IS_NUM(a)) {
      if (IS_NAN(a) || IS_NAN(b)) return JSBOOL(0);
      if (a->number.val == b->number.val) return JSBOOL(1);
      return JSBOOL(0);
    }
    if (IS_STR(a))
      return JSBOOL(STREQ(a->string.ptr, b->string.ptr));
    if (IS_BOOL(a))
      return JSBOOL(a->boolean.val == b->boolean.val);
    // Functions & Objects (must be same ref)
    return JSBOOL(a == b);
  }

  // null == undefined
  if ((IS_UNDEF(a) && IS_NULL(b)) || (IS_NULL(a) && IS_UNDEF(b)))
    return JSBOOL(1);

  // Convert and try again
  if ((IS_NUM(a) && IS_STR(b)) || (IS_STR(a) && IS_NUM(b)))
    return fh_eq(TO_NUM(a), TO_NUM(b), strict);
  if (IS_BOOL(a))
    return fh_eq(TO_NUM(a), b, strict);
  if (IS_BOOL(b))
    return fh_eq(TO_NUM(b), a, strict);
  if ((IS_STR(a) || IS_NUM(a)) && IS_OBJ(b))
    return fh_eq(a, fh_to_primitive(b, T_NUMBER), strict);
  if (IS_OBJ(a) && (IS_STR(b) || IS_NUM(b)))
    return fh_eq(fh_to_primitive(a, T_NUMBER), b, strict);

  return JSBOOL(0);
}

js_val *
fh_neq(js_val *a, js_val *b, bool strict)
{
  // Invert the result of fh_eq
  return JSBOOL(!fh_eq(a, b, strict)->boolean.val);
}

js_val *
fh_abstr_rel_comp(js_val *a, js_val *b, bool left_first)
{
  // Abstract Relational Comparison Algorithm (ECMA 11.5.8)

  // These conversions may have side effects. It's important that
  // left-to-right evaluation is preserved.
  if (left_first) {
    a = fh_to_primitive(a, T_NUMBER);
    b = fh_to_primitive(b, T_NUMBER);
  }
  else {
    b = fh_to_primitive(b, T_NUMBER);
    a = fh_to_primitive(a, T_NUMBER);
  }

  if (IS_STR(a) && IS_STR(b)) {
    return JSBOOL(strcmp(a->string.ptr, b->string.ptr) < 0);
  }

  a = TO_NUM(a), b = TO_NUM(b);

  if (IS_NAN(a) || IS_NAN(b)) return JSUNDEF();
  if (a->number.val == b->number.val) return JSBOOL(0);
  if (IS_INF(a) && !a->number.is_neg) return JSBOOL(0);
  if (IS_INF(b) && !b->number.is_neg) return JSBOOL(1);
  if (IS_INF(b) && b->number.is_neg) return JSBOOL(0);
  if (IS_INF(a) && a->number.is_neg) return JSBOOL(1);
  return JSBOOL(a->number.val < b->number.val);
}

js_val *
fh_lt(js_val *a, js_val *b, bool or_equal)
{
  js_val *res;
  if (or_equal) {
    res = fh_abstr_rel_comp(b, a, false);
    return JSBOOL(!(IS_UNDEF(res) || res->boolean.val));
  }
  res = fh_abstr_rel_comp(a, b, true);
  return IS_UNDEF(res) ? JSBOOL(0): res;
}

js_val *
fh_gt(js_val *a, js_val *b, bool or_equal)
{
  js_val *res;
  if (or_equal) {
    res = fh_abstr_rel_comp(a, b, true);
    return JSBOOL(!(IS_UNDEF(res) || res->boolean.val));
  }
  res = fh_abstr_rel_comp(b, a, false);
  return IS_UNDEF(res) ? JSBOOL(0): res;
}

js_val *
fh_and(js_val *ctx, ast_node *a, ast_node *b)
{
  // && operator returns the first false value, or the second true value. 
  js_val *aval = fh_eval(ctx, a);
  if (TO_BOOL(aval)->boolean.val) return fh_eval(ctx, b);
  return aval;
}

js_val *
fh_or(js_val *ctx, ast_node *a, ast_node *b)
{
  // || returns the first true value, or the second false value.
  js_val *aval = fh_eval(ctx, a);
  if (TO_BOOL(aval)->boolean.val) return aval;
  return fh_eval(ctx, b);
}


// ----------------------------------------------------------------------------
// Utilities
// ----------------------------------------------------------------------------

js_val *
fh_str_from_node(js_val *ctx, ast_node *node)
{
  // Need to evaluate the node to a string key.
  // e.g. obj['a']  obj.a  arr[1]  arr[arr.length - 1]

  if (node->type == NODE_IDENT)
    return JSSTR(node->sval);
  if (node->type == NODE_VAR_DEC)
    return fh_str_from_node(ctx, node->e1);
  return TO_STR(fh_eval(ctx, node));
}

js_val *
fh_eval_each(js_val *ctx, ast_node *node)
{
  js_val *result = JSUNDEF();
  rewind_node(node);
  while (!node->visited) result = fh_eval(ctx, pop_node(node));
  return result;
}
