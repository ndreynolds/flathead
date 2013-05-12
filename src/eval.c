/*
 * eval.c -- AST-walking interpreter
 *
 * Copyright (c) 2012-2013 Nick Reynolds
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


// ----------------------------------------------------------------------------
// Utilities
// ----------------------------------------------------------------------------

static js_val *
eval_each(js_val *ctx, ast_node *node)
{
  js_val *result = JSUNDEF();
  node_rewind(node);
  while (!node->visited) result = fh_eval(ctx, node_pop(node));
  return result;
}

static js_val *
str_from_node(js_val *ctx, ast_node *node)
{
  // Need to evaluate the node to a string key.
  // e.g. obj['a']  obj.a  arr[1]  arr[arr.length - 1]

  if (node->type == NODE_IDENT)
    return JSSTR(node->sval);
  if (node->type == NODE_VAR_DEC)
    return str_from_node(ctx, node->e1);
  return TO_STR(fh_eval(ctx, node));
}


// ----------------------------------------------------------------------------
// Lookups, Sets & Member Expressions
// ----------------------------------------------------------------------------

static js_val * member_exp(js_val *, ast_node *);

static js_val *
member_parent(js_val *ctx, ast_node *member)
{
  return member->e2->type == NODE_MEMBER ? 
    member_exp(ctx, member->e2) :
    fh_eval(ctx, member->e2);
}

static js_val *
member_child(js_val *ctx, ast_node *member)
{
  // In `x.foo` we'll take 'foo' literally, in `x[foo]` we need to eval 'foo'.
  // This distinction is stored as 0/1 in the val slot.
  return member->val ? 
    TO_STR(fh_eval(ctx, member->e1)) :
    str_from_node(ctx, member->e1);
}

static js_val *
member_exp(js_val *ctx, ast_node *member)
{
  js_val *parent = member_parent(ctx, member);
  js_val *child_name = member_child(ctx, member);

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

static js_val *
ident(js_val *ctx, ast_node *id)
{
  js_prop *prop = fh_get_prop_rec(ctx, id->sval);
  if (!prop) {
    eval_state *state = fh_new_state(id->line, id->column);
    fh_push_state(state);
    fh_throw(state, fh_new_error(E_REFERENCE, "%s is not defined", id->sval));
  }
  return prop->ptr;
}

static void
put(js_val *ctx, ast_node *ref, js_val *val)
{
  char *key;
  if (ref->type == NODE_MEMBER) {
    ctx = member_parent(ctx, ref);
    key = member_child(ctx, ref)->string.ptr;
  }
  else if (ref->sval)
    key = ref->sval;
  else
    return;

  fh_set_rec(ctx, key, val);
}


// ----------------------------------------------------------------------------
// Operators
// ----------------------------------------------------------------------------

static js_val *
add_op(js_val *a, js_val *b)
{
  a = fh_to_primitive(a, T_NUMBER);
  b = fh_to_primitive(b, T_NUMBER);

  if (IS_STR(a) || IS_STR(b))
    return JSSTR(fh_str_concat(TO_STR(a)->string.ptr, TO_STR(b)->string.ptr));

  if (!IS_NUM(a) || !IS_NUM(b))
    return add_op(TO_NUM(a), TO_NUM(b));

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

static js_val *
sub_op(js_val *a, js_val *b)
{
  // a - b  <==>  a + (-b)
  return add_op(TO_NUM(a), JSNUM(-1 * TO_NUM(b)->number.val));
}

static js_val *
mul_op(js_val *a, js_val *b)
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

static js_val *
div_op(js_val *a, js_val *b)
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
  return div_op(TO_NUM(a), TO_NUM(b));
}

static js_val *
mod_op(js_val *a, js_val *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    return JSNUM(fmod(a->number.val, b->number.val));
  }
  return mod_op(TO_NUM(a), TO_NUM(b));
}

static js_val *
eq_op(js_val *a, js_val *b, bool strict)
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
    return eq_op(TO_NUM(a), TO_NUM(b), strict);
  if (IS_BOOL(a))
    return eq_op(TO_NUM(a), b, strict);
  if (IS_BOOL(b))
    return eq_op(TO_NUM(b), a, strict);
  if ((IS_STR(a) || IS_NUM(a)) && IS_OBJ(b))
    return eq_op(a, fh_to_primitive(b, T_NUMBER), strict);
  if (IS_OBJ(a) && (IS_STR(b) || IS_NUM(b)))
    return eq_op(fh_to_primitive(a, T_NUMBER), b, strict);

  return JSBOOL(0);
}

js_val *
fh_eq(js_val *a, js_val *b, bool strict)
{
  return eq_op(a, b, strict);
}

static js_val *
neq_op(js_val *a, js_val *b, bool strict)
{
  // Invert the result of eq_op
  return JSBOOL(!eq_op(a, b, strict)->boolean.val);
}

static js_val *
abstr_rel_comp(js_val *a, js_val *b, bool left_first)
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

static js_val *
lt_op(js_val *a, js_val *b, bool or_equal)
{
  js_val *res;
  if (or_equal) {
    res = abstr_rel_comp(b, a, false);
    return JSBOOL(!(IS_UNDEF(res) || res->boolean.val));
  }
  res = abstr_rel_comp(a, b, true);
  return IS_UNDEF(res) ? JSBOOL(0): res;
}

static js_val *
gt_op(js_val *a, js_val *b, bool or_equal)
{
  js_val *res;
  if (or_equal) {
    res = abstr_rel_comp(a, b, true);
    return JSBOOL(!(IS_UNDEF(res) || res->boolean.val));
  }
  res = abstr_rel_comp(b, a, false);
  return IS_UNDEF(res) ? JSBOOL(0): res;
}

static js_val *
and_exp(js_val *ctx, ast_node *a, ast_node *b)
{
  // && operator returns the first false value, or the second true value. 
  js_val *aval = fh_eval(ctx, a);
  if (TO_BOOL(aval)->boolean.val) return fh_eval(ctx, b);
  return aval;
}

static js_val *
or_exp(js_val *ctx, ast_node *a, ast_node *b)
{
  // || returns the first true value, or the second false value.
  js_val *aval = fh_eval(ctx, a);
  if (TO_BOOL(aval)->boolean.val) return aval;
  return fh_eval(ctx, b);
}

static js_val *
delete_op(js_val *ctx, ast_node *node)
{
  js_val *name = str_from_node(ctx, node->e1), 
         *env = ctx;

  if (node->e1->type == NODE_MEMBER) {
    env = member_parent(ctx, node->e1);
    name = member_child(ctx, node->e1);
  }
  else {
    name = str_from_node(ctx, node->e1);
  }

  js_prop *prop = fh_get_prop(env, name->string.ptr);
  if (!prop->configurable)
    return JSBOOL(0);
  return JSBOOL(fh_del_prop(env, name->string.ptr));
}


// ----------------------------------------------------------------------------
// Declaration & Assignment
// ----------------------------------------------------------------------------

static void 
assign(js_val *obj, char *name, js_val *val, char *op)
{
  if (STREQ(op, "=")) { 
    fh_set_rec(obj, name, val);
    return;
  }

  // Handle other assignment operators
  js_val *cur = fh_get_rec(obj, name);
  if (STREQ(op, "+=")) fh_set_rec(obj, name, add_op(cur, val));
  if (STREQ(op, "-=")) fh_set_rec(obj, name, sub_op(cur, val));
  if (STREQ(op, "*=")) fh_set_rec(obj, name, mul_op(cur, val));
  if (STREQ(op, "/=")) fh_set_rec(obj, name, div_op(cur, val));
}

static js_val *
assign_exp(js_val *ctx, ast_node *node)
{
  js_val *val = fh_eval(ctx, node->e2);
  char *key = node->e1->sval;

  if (node->e1->type == NODE_MEMBER) {
    js_val *old_ctx = ctx; 
    ctx = member_parent(old_ctx, node->e1);
    key = member_child(old_ctx, node->e1)->string.ptr;

    // Set the array length.
    if (IS_ARR(ctx)) {
      char *err;
      unsigned long val = strtod(key, &err);
      if (*err == 0 && val >= ctx->object.length)
          fh_set_len(ctx, val + 1);
    }
  }

  if (IS_OBJ(ctx))
    assign(ctx, key, val, node->sval);
  return val;
}

static js_val *
var_dec(js_val *ctx, ast_node *node, bool ignore_rval)
{
  // If node->val is set, the variable declaration has already been hoisted and
  // should not be touched.
  if (!node->val) {
    if (node->e2 == NULL || ignore_rval)
      fh_set_prop(ctx, node->e1->sval, JSUNDEF(), P_WRITE | P_ENUM);
    else
      fh_set_prop(ctx, node->e1->sval, fh_eval(ctx, node->e2), P_WRITE | P_ENUM);
  }
  // If it has already been hoisted, we may still need to do the assignment.
  else if (node->e2) {
    node->sval = "=";
    assign_exp(ctx, node);
  }
  return JSUNDEF();
}


// ----------------------------------------------------------------------------
// Control
// ----------------------------------------------------------------------------

static js_val *
return_stmt(js_val *ctx, ast_node *node)
{
  js_val *result = node->e1 ? fh_eval(ctx, node->e1) : JSUNDEF();
  if (IS_FUNC(result))
    result->object.scope = ctx;
  result->signal = S_BREAK;
  return result;
}

static js_val *
break_stmt()
{
  js_val *signal = JSNULL();
  signal->signal = S_BREAK;
  return signal;
}

static js_val *
if_stmt(js_val *ctx, ast_node *node)
{
  if (TO_BOOL(fh_eval(ctx, node->e1))->boolean.val)
    return fh_eval(ctx, node->e2);
  else if (node->e3 != NULL)
    return fh_eval(ctx, node->e3);
  return JSUNDEF();
}

static js_val *
switch_stmt(js_val *ctx, ast_node *node)
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
        current = node_pop(clauses);
        val = fh_eval(ctx, current->e1);
        // Cases fall-through to the next when breaks are omitted.
        if (matched || eq_op(test, val, true)->boolean.val) {
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
// Source & Statement Lists
// ----------------------------------------------------------------------------

static void
func_dec_scan(js_val *ctx, ast_node *node)
{
  // Sweep for function declarations
  ast_node *child;
  while (!node->visited) {
    child = node_pop(node);
    if (child->type == NODE_FUNC) {
      char *name = str_from_node(ctx, child->e3)->string.ptr;
      fh_set_prop(ctx, name, JSFUNC(child), P_WRITE | P_ENUM);
    }
  }
  node_rewind(node);
}

static void
var_dec_scan(js_val *ctx, ast_node *node)
{
  // Sweep for variable declarations

  // Don't touch functions (stay within our current scope)
  if (node->type == NODE_FUNC) return;

  // Declare the variable and possibly convert it to an assignment
  if (node->type == NODE_VAR_DEC) {
    var_dec(ctx, node, true);    // true to ignore rhs
    node->val = 1;               // Signals that it has been hoisted
  }

  // Recurse sub nodes, will hit the whole tree.
  if (node->e1) var_dec_scan(ctx, node->e1);
  if (node->e2) var_dec_scan(ctx, node->e2);
  if (node->e3) var_dec_scan(ctx, node->e3);
}

static js_val *
stmt_lst(js_val *ctx, ast_node *node)
{
  js_val *result = NULL;
  ast_node *child;
  node_rewind(node);
  while (!node->visited) {
    child = node_pop(node);

    if (child->type == NODE_RETURN)
      return return_stmt(ctx, child);
    if (child->type == NODE_BREAK)
      return break_stmt();
    if (child->type == NODE_CONT)
      return JSUNDEF();

    // Break signals bubble up as special values with a signal flag.
    result = fh_eval(ctx, child);
    if (result->signal == S_BREAK)
      return result;
  }
  return result ? result : JSUNDEF();
}

static js_val *
src_lst(js_val *ctx, ast_node *node)
{
  func_dec_scan(ctx, node);
  var_dec_scan(ctx, node);

  return stmt_lst(ctx, node);
}


// ----------------------------------------------------------------------------
// Object & Array Literals
// ----------------------------------------------------------------------------

static js_val *
obj_lit(js_val *ctx, ast_node *node)
{
  js_val *obj = JSOBJ();
  obj->object.parent = ctx;
  if (node->e1 != NULL) fh_eval(obj, node->e1);
  return obj;
}

static js_val *
arr_lit(js_val *ctx, ast_node *node)
{
  js_val *arr = JSARR();
  if (node->e1 != NULL) {
    int i = 0;
    while (!node->e1->visited) {
      fh_set(arr, JSNUMKEY(i++)->string.ptr, fh_eval(ctx, node_pop(node->e1)));
    }
    fh_set_len(arr, i);
  }
  return arr;
}


// ----------------------------------------------------------------------------
// Iteration Constructs
// ----------------------------------------------------------------------------

static void
while_stmt(js_val *ctx, ast_node *cnd, ast_node *stmt)
{
  js_val *result;

  while (TO_BOOL(fh_eval(ctx, cnd))->boolean.val) {
    result = fh_eval(ctx, stmt);
    if (result->signal == S_BREAK) break;
  }
}

static void
for_stmt(js_val *ctx, ast_node *exp_grp, ast_node *stmt)
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

static void
forin_stmt(js_val *ctx, ast_node *node)
{
  js_val *result, *obj, *env, *name;

  obj = fh_eval(ctx, node->e2);
  env = ctx;

  if (node->e1->type == NODE_MEMBER) {
    env = member_parent(ctx, node->e1);
    name = member_child(ctx, node->e1);
  }
  else 
    name = str_from_node(ctx, node->e1);

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
// Try/Catch
// ----------------------------------------------------------------------------

static js_val *
try_stmt(js_val *ctx, ast_node *node)
{
  eval_state *state = fh_new_state(node->line, node->column);
  state->catch = true;
  state->ctx = ctx;
  fh_push_state(state);

  // Try
  if (!setjmp(state->jmp))
    fh_eval(ctx, node->e1);
  // Catch
  else {
    fh_set(ctx, node->e2->e1->sval, fh_get(ctx, "FH_LAST_ERROR")); 
    fh_eval(ctx, node->e2->e2);
  }

  // Finally
  if (node->e3 && node->e3->e1)
    fh_eval(ctx, node->e3->e1);

  return JSUNDEF();
}

static js_val *
throw_stmt(js_val *ctx, ast_node *exp)
{
  eval_state *state = fh_new_state(exp->line, exp->column);
  fh_push_state(state);
  js_val *val = fh_eval(ctx, exp);
  fh_throw(state, val);
  return JSUNDEF();
}


// ----------------------------------------------------------------------------
// Function Application
// ----------------------------------------------------------------------------

static js_val *
setup_call_env(js_val *ctx, js_val *this, js_val *func, js_args *args)
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
  unsigned long i, arglen = ARGLEN(args);
  for (i = 0; i < arglen; i++)
    fh_set(arguments, JSNUMKEY(i)->string.ptr, ARG(args, i));
  fh_set_class(arguments, "Arguments");
  fh_set(arguments, "callee", func);
  fh_set(arguments, "length", JSNUM(arglen));

  // Set up params as locals (if any)
  if (func_node->e1 != NULL) {
    ast_node *params = func_node->e1;
    node_rewind(params);
    // Go through each param and match it by position with an arg.
    while (!params->visited) {
      if (args->arg) {
        fh_set(scope, node_pop(params)->sval, args->arg);
        if (args->next)
          args = args->next;
        else
          args->arg = NULL;
      }
      else {
        fh_set(scope, node_pop(params)->sval, JSUNDEF());
      }
    }
  }
  return scope;
}

static js_args *
build_args(js_val *ctx, ast_node *args_node)
{
  js_args *args = args_new();

  if (args_node->e1 == NULL) return args;
  if (!args_node->visited) {
    args->arg = fh_eval(ctx, node_pop(args_node));
    args->next = args_node->visited ? NULL : build_args(ctx, args_node);
  }
  node_rewind(args_node);
  return args;
}

static js_val *
call(js_val *ctx, js_val *this, js_val *func, eval_state *state, js_args *args)
{
  if (IS_UNDEF(this) || IS_NULL(this)) 
    this = fh->global;

  state->ctx = ctx;
  state->this = this;

  if (func->object.native) {
    // Native functions are C functions referenced by pointer.
    js_native_function *native = func->object.nativefn;
    js_val *instance = func->object.instance;
    state->caller_info = "(built-in function)";

    // new Number, new Boolean, etc. return wrapper objects. 
    // Here we resolve the wrapper to the value it wraps.
    if (instance && IS_OBJ(instance) && instance->object.primitive)
      instance = instance->object.primitive;

    return native(instance, args, state);
  }

  node_rewind(func->object.node);

  if (func->object.node->e3 && func->object.node->e3->sval)
    state->caller_info = func->object.node->e3->sval;
  else
    state->caller_info = "(anonymous function)";

  js_val *func_scope = setup_call_env(ctx, this, func, args);
  return fh_eval(func_scope, func->object.node->e2);
}

static js_val *
call_exp(js_val *ctx, ast_node *node)
{
  js_val *maybe_func = fh_eval(ctx, node->e1);

  // Special treatment for:
  //   CallExpression [ Expression ] 
  //   CallExpression . Identifier
  if (node->e2->type != NODE_ARG_LST)
    return fh_get_proto(maybe_func, str_from_node(ctx, node->e2)->string.ptr);

  eval_state *state = fh_new_state(node->line, node->column);

  state->ctx = ctx;
  if (!IS_FUNC(maybe_func))
    fh_throw(state, fh_new_error(E_TYPE, "%s is not a function", fh_typeof(maybe_func)));
  js_args *args = build_args(ctx, node->e2);

  // Check for a bound this (see Function#bind)
  js_val *this = maybe_func->object.bound_this ? 
    maybe_func->object.bound_this : fh_get(ctx, "this");

  fh_push_state(state);
  js_val *res = call(ctx, this, maybe_func, state, args);
  fh_pop_state();
  return res;
}

js_val *
fh_call(js_val *ctx, js_val *this, js_val *func, js_args *args)
{
  int line = func->object.native ? 0 : func->object.node->line;
  int column = func->object.native ? 0 : func->object.node->column;
  eval_state *state = fh_new_state(line, column);

  fh_push_state(state);
  js_val *res = call(ctx, this, func, state, args);
  fh_pop_state();

  return res;
}


// ----------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------

static js_val *
new_exp(js_val *ctx, ast_node *exp)
{
  js_val *ctr;
  js_args *args;

  // new F(x, y, z)
  if (exp->e1 && exp->e1->type == NODE_MEMBER) {
    ctr = fh_eval(ctx, exp->e1->e2);
    args = build_args(ctx, exp->e1->e1);
  }
  // new F
  else {
    ctr = fh_eval(ctx, exp->e1);
    args = args_new();
  }

  eval_state *state = fh_new_state(exp->line, exp->column);
  state->construct = true;

  if (!IS_FUNC(ctr))
    fh_throw(state, fh_new_error(E_TYPE, "%s is not a function", fh_typeof(ctr)));

  js_val *res, *obj = JSOBJ(), *proto = fh_get(ctr, "prototype");

  fh_push_state(state);
  res = call(ctx, obj, ctr, state, args); 
  fh_pop_state();
  res = IS_OBJ(res) ? res : obj;

  // Automatically set the prototype to use the constructor's "prototype"
  // property if valid and the result's proto member appears to be unmodified.
  if (IS_OBJ(proto) && res->proto == fh->object_proto)
    res->proto = IS_OBJ(proto) ? proto : fh->object_proto;

  return res;
}


// ----------------------------------------------------------------------------
// Unary Expressions
// ----------------------------------------------------------------------------

static js_val *
postfix_exp(js_val *ctx, ast_node *node)
{
  js_val *old_val = TO_NUM(fh_eval(ctx, node->e1));
  char *op = node->sval;
  if (STREQ(op, "++")) {
    put(ctx, node->e1, add_op(old_val, JSNUM(1)));
    return old_val;
  }
  if (STREQ(op, "--")) {
    put(ctx, node->e1, sub_op(old_val, JSNUM(1)));
    return old_val;
  }
  UNREACHABLE();
}

static js_val *
prefix_exp(js_val *ctx, ast_node *node)
{
  char *op = node->sval;

  if (STREQ(op, "delete"))
    return delete_op(ctx, node);
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
  // TODO: these need to throw a syntax error for strict references
  if (STREQ(op, "++")) {
    new_val = add_op(old_val, JSNUM(1));
    put(ctx, node->e1, new_val);
    return new_val;
  }
  if (STREQ(op, "--")) {
    new_val = sub_op(old_val, JSNUM(1));
    put(ctx, node->e1, new_val);
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


// ----------------------------------------------------------------------------
// Binary Expressions
// ----------------------------------------------------------------------------

static js_val *
bin_exp(js_val *ctx, ast_node *node)
{
  char *op = node->sval;

  // Logical (must short-circuit)
  if (STREQ(op, "&&")) return and_exp(ctx, node->e1, node->e2);
  if (STREQ(op, "||")) return or_exp(ctx, node->e1, node->e2);

  // At this point, we can safely evaluate both expressions.
  js_val *a = fh_eval(ctx, node->e1);
  js_val *b = fh_eval(ctx, node->e2);

  // Arithmetic and string operations
  if (STREQ(op, "+")) return add_op(a, b);
  if (STREQ(op, "-")) return sub_op(a, b);
  if (STREQ(op, "*")) return mul_op(a, b); 
  if (STREQ(op, "/")) return div_op(a, b);
  if (STREQ(op, "%")) return mod_op(a, b);

  // (In)equality
  if (STREQ(op, "=="))  return eq_op(a, b, false);
  if (STREQ(op, "!="))  return neq_op(a, b, false);
  if (STREQ(op, "===")) return eq_op(a, b, true);
  if (STREQ(op, "!==")) return neq_op(a, b, true);

  // Relational 
  if (STREQ(op, "<"))  return lt_op(a, b, false);
  if (STREQ(op, ">"))  return gt_op(a, b, false);
  if (STREQ(op, "<=")) return lt_op(a, b, true);
  if (STREQ(op, ">=")) return gt_op(a, b, true);
  if (STREQ(op, "instanceof")) {
    if (!IS_FUNC(b)) {
      char *fmt = "Expecting a function in 'instanceof' check, got %s";
      fh_throw(NULL, fh_new_error(E_TYPE, fmt, fh_typeof(b)));
    }
    return fh_has_instance(b, a);
  }
  if (STREQ(op, "in")) {
    if (!IS_OBJ(b)) {
      char *fmt = "Expecting an object with 'in' operator, got %s";
      fh_throw(NULL, fh_new_error(E_TYPE, fmt, fh_typeof(b)));
    }
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


// ----------------------------------------------------------------------------
// Expressions
// ----------------------------------------------------------------------------

static js_val *
base_exp(js_val *ctx, ast_node *node)
{
  switch (node->sub_type) {
    case NODE_UNARY_POST: 
      return postfix_exp(ctx, node);
    case NODE_UNARY_PRE:
      return prefix_exp(ctx, node);
    default:
      return bin_exp(ctx, node);
  }
}


// ----------------------------------------------------------------------------
// Evaluation
// ----------------------------------------------------------------------------

js_val * 
fh_eval(js_val *ctx, ast_node *node)
{
  if (!node) return JSUNDEF();

  switch (node->type) {
    case NODE_BOOL:        return JSBOOL(node->val);
    case NODE_STR:         return JSSTR(node->sval);
    case NODE_REGEXP:      return JSRE(node->sval);
    case NODE_NUM:         return JSNUM(node->val);
    case NODE_NULL:        return JSNULL();
    case NODE_FUNC:        return JSFUNC(node);

    case NODE_OBJ:         return obj_lit(ctx, node);
    case NODE_ARR:         return arr_lit(ctx, node);
    case NODE_CALL:        return call_exp(ctx, node);
    case NODE_NEW:         return new_exp(ctx, node);
    case NODE_MEMBER:      return member_exp(ctx, node);
    case NODE_IDENT:       return ident(ctx, node);
    case NODE_STMT_LST:    return stmt_lst(ctx, node);
    case NODE_SRC_LST:     return src_lst(ctx, node);
    case NODE_EXP:         return base_exp(ctx, node);
    case NODE_SWITCH_STMT: return switch_stmt(ctx, node);
    case NODE_ASGN:        return assign_exp(ctx, node);
    case NODE_RETURN:      return return_stmt(ctx, node);
    case NODE_VAR_DEC:     return var_dec(ctx, node, false);
    case NODE_BREAK:       return break_stmt();
    case NODE_TRY_STMT:    return try_stmt(ctx, node);
    case NODE_THROW:       return throw_stmt(ctx, node->e1);
    case NODE_IF:          return if_stmt(ctx, node);
    case NODE_TERN:        return if_stmt(ctx, node);
    case NODE_BLOCK:       return fh_eval(ctx, node->e1);
    case NODE_EXP_STMT:    return fh_eval(ctx, node->e1);
    case NODE_VAR_STMT:    return fh_eval(ctx, node->e1);
    case NODE_VAR_DEC_LST: return eval_each(ctx, node);
    case NODE_PROP_LST:    return eval_each(ctx, node);

    case NODE_PROP:        fh_set(ctx, node->e1->sval, fh_eval(ctx, node->e2)); break;
    case NODE_WHILE:       while_stmt(ctx, node->e1, node->e2); break;
    case NODE_FOR:         for_stmt(ctx, node->e1, node->e2); break;
    case NODE_FORIN:       forin_stmt(ctx, node); break;
    case NODE_EMPT_STMT:   break;

    default: 
    {
      eval_state *state = fh_new_state(node->line, node->column);
      fh_push_state(state);
      fh_throw(state, fh_new_error(E_SYNTAX, "Unsupported syntax type (%d)", node->type));
    }
  }

  return JSUNDEF();
}
