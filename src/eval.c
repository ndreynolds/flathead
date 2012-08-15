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

JSVALUE * 
fh_eval(JSVALUE *ctx, JSNODE *node)
{
  JSVALUE *result = JSUNDEF();
  switch(node->type) {
    case NODE_BOOL: return JSBOOL((bool)node->val);
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
    case NODE_EXP_STMT: return fh_eval(ctx, node->e1);
    case NODE_EMPT_STMT: break;
    case NODE_EXP:
      if (node->sub_type == NODE_UNARY_POST) 
        return fh_eval_postfix_exp(ctx, node);
      else if (node->sub_type == NODE_UNARY_PRE)
        return fh_eval_prefix_exp(ctx, node);
      else
        return fh_eval_bin_exp(ctx, node);
    case NODE_ASGN:
      // TODO: This assumes the Node at e1 is an ident, which won't
      // always be true. 
      fh_assign(
        ctx, 
        node->e1->sval,
        fh_eval(ctx, node->e2),
        node->sval
      );
      return fh_eval(ctx, node->e2);
    case NODE_IF:
      if (JSCAST(fh_eval(ctx, node->e1), T_BOOLEAN)->boolean.val)
        return fh_eval(ctx, node->e2);
      else if (node->e3 != NULL) 
        return fh_eval(ctx, node->e3);
    case NODE_PROP_LST:
      rewind_node(node);
      while(!node->visited) result = fh_eval(ctx, pop_node(node));
      break;
    case NODE_PROP:
      fh_set(ctx, node->e1->sval, fh_eval(ctx, node->e2));
      break;
    case NODE_VAR_STMT:
      if (node->e2 != NULL)
        fh_set(ctx, node->e1->sval, fh_eval(ctx, node->e2));
      else
        fh_set(ctx, node->e1->sval, JSNULL());
      break;
    case NODE_WHILE:
      fh_while(ctx, node->e1, node->e2);
      break;
    case NODE_RETURN:
      result = node->e1 == NULL ? JSUNDEF() : fh_eval(ctx, node->e1);
      if (result->type == T_FUNCTION) 
        result->function.closure = ctx;
      return result;
    default:
      fprintf(stderr, "Unsupported syntax type (%d)\n", node->type);
      exit(1);
  }
  return result;
}

JSVALUE *
fh_stmt_lst(JSVALUE *ctx, JSNODE *node)
{
  JSVALUE *result;
  JSNODE *child;
  rewind_node(node);
  while(!node->visited) {
    child = pop_node(node);
    if (child->type == NODE_RETURN) {
      if (child->e1 == NULL) return JSUNDEF();
      JSVALUE *val = fh_eval(ctx, child);
      if (val->type == T_FUNCTION) val->function.closure = ctx;
      return val;
    }
    result = fh_eval(ctx, child);
  }
  return result;
}

void
fh_while(JSVALUE *ctx, JSNODE *cnd, JSNODE *block)
{
  while(JSCAST(fh_eval(ctx, cnd), T_BOOLEAN)->boolean.val) {
    fh_eval(ctx, block);
  }
}

JSVALUE *
fh_obj(JSVALUE *ctx, JSNODE *node)
{
  JSVALUE *obj = JSOBJ();
  obj->object.parent = ctx;
  if (node->e1 != NULL) fh_eval(obj, node->e1);
  return obj;
}

JSVALUE *
fh_arr(JSVALUE *ctx, JSNODE *node)
{
  JSVALUE *obj = JSOBJ();
  obj->object.is_array = true;
  if (node->e1 != NULL) {
    int i = 0;
    JSVALUE *str;
    while(!node->e1->visited) {
      str = JSCAST(JSNUM(i), T_STRING);
      fh_set(obj, str->string.ptr, fh_eval(ctx, pop_node(node->e1)));
      i++;
    }
  }
  return obj;
}

JSVALUE *
fh_member(JSVALUE *ctx, JSNODE *member)
{
  // Similar to an Identifier node, we have to retrieve a value
  // from an object. The difference here is that we need to recurse
  // to retrieve sub-members as instructed.
  // TODO
  JSVALUE *a = fh_get(ctx, member->e2->sval);
  JSVALUE *b = fh_get(a, member->e1->sval);
  return b;
}

void 
fh_assign(JSVALUE *obj, char *name, JSVALUE *val, char *op)
{
  if (STREQ(op, "=")) return fh_set_rec(obj, name, val);

  // Handle other assignment operators
  JSVALUE *cur = fh_get_rec(obj, name);
  if (STREQ(op, "+=")) return fh_set(obj, name, fh_add(cur, val));
  if (STREQ(op, "-=")) return fh_set(obj, name, fh_sub(cur, val));
  if (STREQ(op, "*=")) return fh_set(obj, name, fh_mul(cur, val));
  if (STREQ(op, "/=")) return fh_set(obj, name, fh_div(cur, val));
}

JSVALUE *
fh_call(JSVALUE *ctx, JSNODE *call)
{
  JSVALUE *maybe_func = fh_eval(ctx, call->e1);
  if (maybe_func->type != T_FUNCTION) {
    fprintf(stderr, "TypeError: %s is not a function\n", fh_typeof(maybe_func));
    exit(1);
  }
  return fh_function_call(ctx, maybe_func, call->e2);
}

JSARGS *
fh_build_args(JSVALUE *ctx, JSNODE *args_node)
{
  JSARGS *args = malloc(sizeof(JSARGS));
  if (args_node->e1 == NULL) return args;
  if (!args_node->visited) {
    args->arg = fh_eval(ctx, pop_node(args_node));
    if (!args_node->visited)
      args->next = fh_build_args(ctx, args_node);
  }
  rewind_node(args_node);
  return args;
}

JSVALUE *
fh_function_call(JSVALUE *ctx, JSVALUE *func, JSNODE *args_node)
{
  JSARGS *args = fh_build_args(ctx, args_node);

  if (func->function.is_native) {
    // Native functions are C functions referenced by pointer.
    rewind_node(args_node);
    JSNATVFUNC native = (JSNATVFUNC)func->function.native;
    return (*native)(args);
  }

  rewind_node(func->function.node);
  JSVALUE *func_scope = fh_setup_func_env(ctx, func, args);
  return fh_eval(func_scope, ((JSNODE *)func->function.node)->e2);
}

JSVALUE *
fh_setup_func_env(JSVALUE *ctx, JSVALUE *func, JSARGS *args)
{
  JSVALUE *arguments = JSOBJ();
  JSNODE *func_node = (JSNODE *)func->function.node;
  JSVALUE *scope = func->function.closure;

  if (scope == NULL) 
    scope = JSOBJ();
  scope->object.parent = ctx;
  fh_set(scope, "arguments", arguments);
  if (func_node->sval != NULL)
    fh_set(scope, func_node->sval, func);

  // Setup the (array-like) arguments object.
  fh_set(arguments, "callee", func);
  int i = 0;
  char *s;
  bool first = true;
  JSARGS *tmp = args;
  while(first || args->next != NULL)
  {
    if (!first)
      args = args->next;
    if (args->arg != NULL) {
      sprintf(s, "%d", i);
      fh_set(arguments, s, args->arg);
      i++;
    }
    first = false;
  }
  args = tmp;
  fh_set(arguments, "length", JSNUM((double)i));

  // Setup params as locals, if any
  if (func_node->e1 != NULL) {
    JSNODE *params = func_node->e1;
    rewind_node(params);
    while(!params->visited) {
      if (args->arg != NULL) {
        fh_set(scope, pop_node(params)->sval, args->arg);
        if (args->next != NULL) args = args->next;
      }
      else {
        fh_set(scope, pop_node(params)->sval, JSUNDEF());
      }
    } 
  }

  return scope;
}

JSVALUE *
fh_eval_postfix_exp(JSVALUE *ctx, JSNODE *node)
{
  JSVALUE *old_val = JSCAST(fh_eval(ctx, node->e1), T_NUMBER);
  char *op = node->sval;
  if (STREQ(op, "++")) {
    fh_set(ctx, node->e1->sval, fh_add(old_val, JSNUM(1)));
    return old_val;
  }
  if (STREQ(op, "--")) {
    fh_set(ctx, node->e1->sval, fh_sub(old_val, JSNUM(1)));
    return old_val;
  }
}

JSVALUE *
fh_eval_prefix_exp(JSVALUE *ctx, JSNODE *node)
{
  char *op = node->sval;
  if (STREQ(op, "+"))
    return JSCAST(fh_eval(ctx, node->e1), T_NUMBER);
  if (STREQ(op, "-"))
    return JSNUM(-1 * JSCAST(fh_eval(ctx, node->e1), T_NUMBER)->number.val);
  if (STREQ(op, "!"))
    return JSCAST(fh_eval(ctx, node->e1), T_BOOLEAN)->boolean.val == 1 ? 
      JSBOOL(0) : JSBOOL(1);

  // Increment and decrement.
  JSVALUE *old_val = JSCAST(fh_eval(ctx, node->e1), T_NUMBER);
  JSVALUE *new_val;
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
}

JSVALUE *
fh_eval_bin_exp(JSVALUE *ctx, JSNODE *node)
{
  char *op = node->sval;

  // Logical (must short-circuit)
  if (STREQ(op, "&&")) return fh_and(ctx, node->e1, node->e2);
  if (STREQ(op, "||")) return fh_or(ctx, node->e1, node->e2);

  // At this point, we can safely evaluate both expressions.
  JSVALUE *a = fh_eval(ctx, node->e1);
  JSVALUE *b = fh_eval(ctx, node->e2);

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
    return fh_lt(a, b)->boolean.val || fh_eq(a, b, false)->boolean.val ?
      JSBOOL(1) : JSBOOL(0);
  if (STREQ(op, ">="))
    return fh_gt(a, b)->boolean.val || fh_eq(a, b, false)->boolean.val ?
      JSBOOL(1) : JSBOOL(0);
}

JSVALUE *
fh_add(JSVALUE *a, JSVALUE *b)
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
}

JSVALUE *
fh_sub(JSVALUE *a, JSVALUE *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    // Subtraction on NaN or Infinity results in NaN
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    return JSNUM(a->number.val - b->number.val);
  }
  return fh_sub(JSCAST(a, T_NUMBER), JSCAST(b, T_NUMBER));
}

JSVALUE *
fh_mul(JSVALUE *a, JSVALUE *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    return JSNUM(a->number.val * b->number.val);
  }
  return fh_mul(JSCAST(a, T_NUMBER), JSCAST(b, T_NUMBER));
}

JSVALUE *
fh_div(JSVALUE *a, JSVALUE *b)
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

JSVALUE *
fh_mod(JSVALUE *a, JSVALUE *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JSNAN();
    if (a->number.is_inf || b->number.is_inf) return JSNAN();
    return JSNUM(fmod(a->number.val, b->number.val));
  }
  return fh_mod(JSCAST(a, T_NUMBER), JSCAST(b, T_NUMBER));
}

JSVALUE *
fh_eq(JSVALUE *a, JSVALUE *b, bool strict)
{
  // Strict equality on different types is always false
  if (a->type != b->type && strict) return JSBOOL(0);

  if (T_XOR(a, b, T_UNDEF, T_NULL)) return JSBOOL(1);
  if (T_BOTH(a, b, T_NULL) || T_BOTH(a, b, T_UNDEF)) return JSBOOL(1);
  if (T_BOTH(a, b, T_STRING))
    return STREQ(a->string.ptr, b->string.ptr) ? JSBOOL(1) : JSBOOL(0);
  if (T_BOTH(a, b, T_NUMBER)) { 
    // Nothing equals NaN
    if (a->number.is_nan || b->number.is_nan) return JSBOOL(0);
    return a->number.val == b->number.val ? JSBOOL(1) : JSBOOL(0);
  }
  // Objects are equal if they occupy the same memory address
  if (T_BOTH(a, b, T_OBJECT))
    return a == b ? JSBOOL(1) : JSBOOL(0);

  return fh_eq(JSCAST(a, T_NUMBER), JSCAST(b, T_NUMBER), false);
}

JSVALUE *
fh_neq(JSVALUE *a, JSVALUE *b, bool strict)
{
  // Invert the result of fh_eq
  return fh_eq(a, b, strict)->boolean.val ? JSBOOL(0) : JSBOOL(1);
}

JSVALUE *
fh_gt(JSVALUE *a, JSVALUE *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    // _ > NaN == false, NaN > _ == false
    if (a->number.is_nan || b->number.is_nan) return JSBOOL(0);
    if (a->number.is_inf) return JSBOOL(1);
    if (b->number.is_inf) return JSBOOL(0);
    return a->number.val > b->number.val ? JSBOOL(1) : JSBOOL(0);
  }
  if (T_BOTH(a, b, T_STRING)) {
    // TODO: Lexicographic comparison
  }
}

JSVALUE *
fh_lt(JSVALUE *a, JSVALUE *b)
{
  // !(a > b || a == b)
  return fh_gt(a, b)->boolean.val || fh_eq(a, b, false)->boolean.val ?
    JSBOOL(0) : JSBOOL(1);
}

JSVALUE *
fh_and(JSVALUE *ctx, JSNODE *a, JSNODE *b)
{
  // && operator returns the first false value, or the second true value. 
  JSVALUE *aval = fh_eval(ctx, a);
  if (JSCAST(aval, T_BOOLEAN)->boolean.val) return fh_eval(ctx, b);
  return aval;
}

JSVALUE *
fh_or(JSVALUE *ctx, JSNODE *a, JSNODE *b)
{
  // || returns the first true value, or the second false value.
  JSVALUE *aval = fh_eval(ctx, a);
  if (JSCAST(aval, T_BOOLEAN)->boolean.val) return aval;
  return fh_eval(ctx, b);
}
