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

JLVALUE * 
jl_eval(JLVALUE *this, JLNode *node)
{
  JLVALUE *result = JLUNDEF();
  switch(node->type) {
    case NODE_BLOCK:
      result = jl_eval(this, node->e1);
      break;
    case NODE_STMT_LST:
      rewind_node(node);
      while(!node->visited) result = jl_eval(this, pop_node(node));
      break;
    case NODE_EXP_STMT:
      return jl_eval_exp(this, node->e1);
    case NODE_IF:
      // If condition is satisfied, evaluate the ifBlock.
      if (JLCAST(jl_eval_exp(this, node->e1), T_BOOLEAN)->boolean.val)
        jl_eval(this, node->e2);
      // Otherwise, provided there is an elseBlock, evaluate it.
      else if (node->e3 != 0) 
        jl_eval(this, node->e3);
      break;
    case NODE_OBJ:
      result = jl_obj(node);
      break;
    case NODE_PROP_LST:
      rewind_node(node);
      while(!node->visited) result = jl_eval(this, pop_node(node));
      break;
    case NODE_PROP:
      jl_set(this, node->e1->sval, jl_eval_exp(this, node->e2));
      break;
    case NODE_VAR_STMT:
      if (node->e2 != 0)
        jl_set(this, node->e1->sval, jl_eval_exp(this, node->e2));
      else
        jl_set(this, node->e1->sval, JLNULL());
      break;
    case NODE_WHILE:
      jl_while(this, node->e1, node->e2);
      break;
    case NODE_RETURN:
      if (node->e1 == 0) return JLUNDEF();
      return jl_eval(this, node->e1);
    case NODE_EMPT_STMT:
      break;
    default:
      return jl_eval_exp(this, node);
  }
  return result;
}

JLVALUE *
jl_eval_exp(JLVALUE *this, JLNode *node)
{
  if (node->type == NODE_BOOL) return JLBOOL((bool)node->val);
  if (node->type == NODE_STR) return JLSTR(node->sval);
  if (node->type == NODE_NUM) return JLNUM(node->val);
  if (node->type == NODE_NULL) return JLNULL();
  if (node->type == NODE_FUNC) return JLFUNC(node);
  if (node->type == NODE_OBJ) return jl_obj(node);
  if (node->type == NODE_CALL) return jl_call(this, node);
  if (node->type == NODE_MEMBER) return jl_member(this, node);
  if (node->type == NODE_IDENT) return jl_get_rec(this, node->sval);
  if (node->type == NODE_ASGN) {
    // This assumes the Node at e1 is an ident, which won't
    // always be true.
    jl_assign(
      this, 
      node->e1->sval,
      jl_eval_exp(this, node->e2),
      node->sval
    );
    return jl_eval_exp(this, node->e2);
  }
  if (node->type == NODE_EXP) {
    switch(node->sub_type) {
      case NODE_UNARY_POST:
        return jl_eval_postfix_exp(this, node);
      case NODE_UNARY_PRE:
        return jl_eval_prefix_exp(this, node);
      default:
        return jl_eval_bin_exp(this, node);
    }
  }
  fprintf(stderr, "Unsupported syntax type (%d)\n", node->type);
  exit(1);
}

void
jl_while(JLVALUE *this, JLNode *cnd, JLNode *block)
{
  int loop_cnt = 0;
  while(JLCAST(jl_eval_exp(this, cnd), T_BOOLEAN)->boolean.val) {
    jl_eval(this, block);
    loop_cnt++;
  }
}

JLVALUE *
jl_obj(JLNode *node)
{
  JLVALUE *obj = JLOBJ();
  if (node->e1) jl_eval(obj, node->e1);
  return obj;
}

JLVALUE *
jl_member(JLVALUE *this, JLNode *member)
{
  // Similar to an Identifier node, we have to retrieve a value
  // from an object. The difference here is that we need to recurse
  // to retrieve sub-members as instructed.
  // TODO
  JLVALUE *a = jl_get(this, member->e2->sval);
  JLVALUE *b = jl_get(a, member->e1->sval);
  return b;
}

void 
jl_assign(JLVALUE *obj, char *name, JLVALUE *val, char *op)
{
  if (STREQ(op, "=")) return jl_set_rec(obj, name, val);

  // Handle other assignment operators
  JLVALUE *cur = jl_get_rec(obj, name);
  if (STREQ(op, "+=")) return jl_set(obj, name, jl_add(cur, val));
  if (STREQ(op, "-=")) return jl_set(obj, name, jl_sub(cur, val));
  if (STREQ(op, "*=")) return jl_set(obj, name, jl_mul(cur, val));
  if (STREQ(op, "/=")) return jl_set(obj, name, jl_div(cur, val));
}

JLVALUE *
jl_call(JLVALUE *this, JLNode *call)
{
  JLVALUE *maybe_func = jl_eval_exp(this, call->e1);
  if (maybe_func->type != T_FUNCTION) {
    fprintf(stderr, "TypeError: %s is not a function\n", jl_typeof(maybe_func));
    exit(1);
  }
  return jl_function_call(this, maybe_func, call->e2);
}

JLARGS *
jl_build_args(JLVALUE *this, JLNode *args_node)
{
  JLARGS *args = malloc(sizeof(JLARGS));
  if (!args_node->visited) {
    args->arg = jl_eval_exp(this, pop_node(args_node));
    if (!args_node->visited)
      args->next = jl_build_args(this, args_node);
  }
  rewind_node(args_node);
  return args;
}

JLVALUE *
jl_function_call(JLVALUE *this, JLVALUE *func, JLNode *args_node)
{
  JLARGS *args = jl_build_args(this, args_node);

  if (func->function.is_native) {
    // Native functions are C functions referenced by pointer.
    rewind_node(args_node);
    JLNATVFUNC native = (JLNATVFUNC)func->function.native;
    return (*native)(args);
  }

  rewind_node(func->function.node);
  JLVALUE *func_scope = jl_setup_func_env(this, func, args);
  return jl_eval(func_scope, ((JLNode *)func->function.node)->e2);
}

JLVALUE *
jl_setup_func_env(JLVALUE *this, JLVALUE *func, JLARGS *args)
{
  JLVALUE *scope = JLOBJ();
  JLVALUE *arguments = JLOBJ();
  JLNode *func_node = (JLNode *)func->function.node;

  jl_set(scope, "this", this);
  jl_set(scope, "arguments", arguments);
  if (func_node->sval != 0)
    jl_set(scope, func_node->sval, func);

  // Setup the (array-like) arguments object.
  jl_set(arguments, "callee", func);
  int i = 0;
  char *s;
  bool first = true;
  JLARGS *tmp = args;
  while(first || args->next != 0)
  {
    if (!first)
      args = args->next;
    if (args->arg != 0) {
      sprintf(s, "%d", i);
      jl_set(arguments, s, args->arg);
      i++;
    }
    first = false;
  }
  args = tmp;
  jl_set(arguments, "length", JLNUM((double)i));

  // Setup params as locals
  JLNode *params = func_node->e1;
  rewind_node(params);
  while(!params->visited) {
    if (args->arg != 0) {
      jl_set(scope, pop_node(params)->sval, args->arg);
      if (args->next != 0) 
        args = args->next;
    }
    else {
      jl_set(scope, pop_node(params)->sval, JLUNDEF());
    }
  } 

  return scope;
}

JLVALUE *
jl_eval_postfix_exp(JLVALUE *this, JLNode *node)
{
  JLVALUE *old_val = JLCAST(jl_eval_exp(this, node->e1), T_NUMBER);
  char *op = node->sval;
  if (STREQ(op, "++")) {
    jl_set(this, node->e1->sval, jl_add(old_val, JLNUM(1)));
    return old_val;
  }
  if (STREQ(op, "--")) {
    jl_set(this, node->e1->sval, jl_sub(old_val, JLNUM(1)));
    return old_val;
  }
}

JLVALUE *
jl_eval_prefix_exp(JLVALUE *this, JLNode *node)
{
  char *op = node->sval;
  if (STREQ(op, "+"))
    return JLCAST(jl_eval_exp(this, node->e1), T_NUMBER);
  if (STREQ(op, "-"))
    return JLNUM(-1 * JLCAST(jl_eval_exp(this, node->e1), T_NUMBER)->number.val);
  if (STREQ(op, "!"))
    return JLCAST(jl_eval_exp(this, node->e1), T_BOOLEAN)->boolean.val == 1 ? 
      JLBOOL(0) : JLBOOL(1);

  // Increment and decrement.
  JLVALUE *old_val = JLCAST(jl_eval_exp(this, node->e1), T_NUMBER);
  JLVALUE *new_val;
  if (STREQ(op, "++")) {
    new_val = jl_add(old_val, JLNUM(1));
    jl_set(this, node->e1->sval, new_val);
    return new_val;
  }
  if (STREQ(op, "--")) {
    new_val = jl_sub(old_val, JLNUM(1));
    jl_set(this, node->e1->sval, new_val);
    return new_val;
  }
}

JLVALUE *
jl_eval_bin_exp(JLVALUE *this, JLNode *node)
{
  char *op = node->sval;

  // Logical (must short-circuit)
  if (STREQ(op, "&&")) return jl_and(this, node->e1, node->e2);
  if (STREQ(op, "||")) return jl_or(this, node->e1, node->e2);

  // At this point, we can safely evaluate both expressions.
  JLVALUE *a = jl_eval_exp(this, node->e1);
  JLVALUE *b = jl_eval_exp(this, node->e2);

  // Arithmetic and string operations
  if (STREQ(op, "+")) return jl_add(a, b);
  if (STREQ(op, "-")) return jl_sub(a, b);
  if (STREQ(op, "*")) return jl_mul(a, b); 
  if (STREQ(op, "/")) return jl_div(a, b);
  if (STREQ(op, "%")) return jl_mod(a, b);

  // (In)equality
  if (STREQ(op, "==")) return jl_eq(a, b, false);
  if (STREQ(op, "!=")) return jl_neq(a, b, false);
  if (STREQ(op, "===")) return jl_eq(a, b, true);
  if (STREQ(op, "!==")) return jl_neq(a, b, true);

  // Relational 
  if (STREQ(op, "<")) return jl_lt(a, b);
  if (STREQ(op, ">")) return jl_gt(a, b);
  if (STREQ(op, "<=")) 
    return jl_lt(a, b)->boolean.val || jl_eq(a, b, false)->boolean.val ?
      JLBOOL(1) : JLBOOL(0);
  if (STREQ(op, ">="))
    return jl_gt(a, b)->boolean.val || jl_eq(a, b, false)->boolean.val ?
      JLBOOL(1) : JLBOOL(0);
}

JLVALUE *
jl_add(JLVALUE *a, JLVALUE *b)
{
  // Two strings => String
  if (T_BOTH(a, b, T_STRING))
    return JLSTR(jl_str_concat(a->string.ptr, b->string.ptr));

  // Two numbers => Number
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JLNAN();
    return JLNUM(a->number.val + b->number.val);
  }

  // String and (null|undefined|Boolean|Number) => String
  if (T_XOR(a, b, T_NULL, T_STRING) || 
      T_XOR(a, b, T_BOOLEAN, T_STRING) ||
      T_XOR(a, b, T_NUMBER, T_STRING) ||
      T_XOR(a, b, T_UNDEF, T_STRING))
    return jl_add(JLCAST(a, T_STRING), JLCAST(b, T_STRING));

  // Number and undefined => NaN
  if (T_XOR(a, b, T_NUMBER, T_UNDEF)) 
    return JLNAN();

  // Number and null => Null
  if (T_XOR(a, b, T_NULL, T_NUMBER))
    return jl_add(JLCAST(a, T_NUMBER), JLCAST(b, T_NUMBER));

  // Number and Boolean => Number
  if (T_XOR(a, b, T_NUMBER, T_BOOLEAN))
    return jl_add(JLCAST(a, T_NUMBER), JLCAST(b, T_NUMBER));
}

JLVALUE *
jl_sub(JLVALUE *a, JLVALUE *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    // Subtraction on NaN or Infinity results in NaN
    if (a->number.is_nan || b->number.is_nan) return JLNAN();
    if (a->number.is_inf || b->number.is_inf) return JLNAN();
    return JLNUM(a->number.val - b->number.val);
  }

  // Simpler approach here. Cast both to Number and recurse.
  // We'll be back at the top with numbers (either finite, infinite, or NaN).
  return jl_sub(JLCAST(a, T_NUMBER), JLCAST(b, T_NUMBER));
}

JLVALUE *
jl_mul(JLVALUE *a, JLVALUE *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JLNAN();
    if (a->number.is_inf || b->number.is_inf) return JLNAN();
    return JLNUM(a->number.val * b->number.val);
  }
  return jl_mul(JLCAST(a, T_NUMBER), JLCAST(b, T_NUMBER));
}

JLVALUE *
jl_div(JLVALUE *a, JLVALUE *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JLNAN();
    if (a->number.is_inf || b->number.is_inf) return JLNAN();
    // Division by 0 yields Infinity
    if (b->number.val == 0) return JLINF();
    return JLNUM(a->number.val / b->number.val);
  }
  return jl_div(JLCAST(a, T_NUMBER), JLCAST(b, T_NUMBER));
}

JLVALUE *
jl_mod(JLVALUE *a, JLVALUE *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JLNAN();
    if (a->number.is_inf || b->number.is_inf) return JLNAN();
    return JLNUM(fmod(a->number.val, b->number.val));
  }
  return jl_mod(JLCAST(a, T_NUMBER), JLCAST(b, T_NUMBER));
}

JLVALUE *
jl_eq(JLVALUE *a, JLVALUE *b, bool strict)
{
  // Strict equality on different types is always false
  if (a->type != b->type && strict) return JLBOOL(0);

  if (T_XOR(a, b, T_UNDEF, T_NULL)) return JLBOOL(1);
  if (T_BOTH(a, b, T_NULL) || T_BOTH(a, b, T_UNDEF)) return JLBOOL(1);
  if (T_BOTH(a, b, T_STRING))
    return STREQ(a->string.ptr, b->string.ptr) ? JLBOOL(1) : JLBOOL(0);
  if (T_BOTH(a, b, T_NUMBER)) { 
    // Nothing equals NaN
    if (a->number.is_nan || b->number.is_nan) return JLBOOL(0);
    return a->number.val == b->number.val ? JLBOOL(1) : JLBOOL(0);
  }
  // Objects are equal if they occupy the same memory address
  if (T_BOTH(a, b, T_OBJECT))
    return a == b ? JLBOOL(1) : JLBOOL(0);

  return jl_eq(JLCAST(a, T_NUMBER), JLCAST(b, T_NUMBER), false);
}

JLVALUE *
jl_neq(JLVALUE *a, JLVALUE *b, bool strict)
{
  // Invert the result of jl_eq
  return jl_eq(a, b, strict)->boolean.val ? JLBOOL(0) : JLBOOL(1);
}

JLVALUE *
jl_gt(JLVALUE *a, JLVALUE *b)
{
  if (T_BOTH(a, b, T_NUMBER)) {
    // _ > NaN == false, NaN > _ == false
    if (a->number.is_nan || b->number.is_nan) return JLBOOL(0);
    if (a->number.is_inf) return JLBOOL(1);
    if (b->number.is_inf) return JLBOOL(0);
    return a->number.val > b->number.val ? JLBOOL(1) : JLBOOL(0);
  }
  if (T_BOTH(a, b, T_STRING)) {
    // TODO: Lexicographic comparison
  }
}

JLVALUE *
jl_lt(JLVALUE *a, JLVALUE *b)
{
  // !(a > b || a == b)
  return jl_gt(a, b)->boolean.val || jl_eq(a, b, false)->boolean.val ?
    JLBOOL(0) : JLBOOL(1);
}

JLVALUE *
jl_and(JLVALUE *this, JLNode *a, JLNode *b)
{
  // && operator returns the first false value, or the second true value. 
  JLVALUE *aval = jl_eval_exp(this, a);
  if (JLCAST(aval, T_BOOLEAN)->boolean.val) return jl_eval_exp(this, b);
  return aval;
}

JLVALUE *
jl_or(JLVALUE *this, JLNode *a, JLNode *b)
{
  // || returns the first true value, or the second false value.
  JLVALUE *aval = jl_eval_exp(this, a);
  if (JLCAST(aval, T_BOOLEAN)->boolean.val) return aval;
  return jl_eval_exp(this, b);
}
