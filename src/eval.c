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
      jl_assign(this, node->e1->sval, jl_eval_exp(this, node->e2));
      break;
    case NODE_VAR_STMT:
      // For now we just initialize the property to null.
      jl_assign(this, node->e1->sval, JLNULL());
      break;
    case NODE_WHILE:
      jl_while(this, node->e1, node->e2);
      break;
    case NODE_RETURN:
      if (node->e1 == 0) return JLUNDEF();
      return jl_eval_exp(this, node->e1);
    case NODE_EMPT_STMT:
      break;
    default:
      fprintf(stderr, "Evaluation error: unsupported structure (%d)\n", node->type);
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
  if (node->type == NODE_IDENT) return jl_get(this, node->sval);
  if (node->type == NODE_ASGN) {
    // This assumes the Node at e1 is an ident, which won't
    // always be true.
    jl_assign(
      this, 
      node->e1->sval,
      jl_eval_exp(this, node->e2)
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
  fprintf(stderr, "Unknown expression type\n");
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
jl_get(JLVALUE *obj, char *prop_name)
{
  // Can't read properties from undefined.
  if (obj->type == T_UNDEF) {
    fprintf(stderr, "TypeError: Cannot read property '%s' of undefined\n", prop_name);
    exit(1);
  }
  JLPROP *prop = jl_lookup(obj, prop_name, true);
  // We'll happily return undefined if a property doesn't exist.
  if (prop == 0) return JLUNDEF();
  return prop->ptr;
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
  return args;
}

JLVALUE *
jl_function_call(JLVALUE *this, JLVALUE *func, JLNode *args_node)
{
  if (func->function.is_native) {
    rewind_node(args_node);
    JLARGS *args = jl_build_args(this, args_node);

    (*(func->function.native))(args);
    // Currently all our native functions (and the type JLNATVFUNC) return void.
    return JLUNDEF();
  } else {
    JLVALUE *scope = JLOBJ();
    jl_assign(scope, "this", this);
    //jl_assign(scope, "arguments", args);
    
    // Gather params
    JLNode *params = ((JLNode *)func->function.body)->e1;
    rewind_node(args_node);
    rewind_node(params);
    while(!params->visited && !args_node->visited)
      jl_assign(
        scope, 
        pop_node(params)->sval, 
        jl_eval_exp(this, pop_node(args_node))
      );
    return jl_eval(scope, ((JLNode *)func->function.body)->e2);
  }
  return JLUNDEF();
}

JLVALUE *
jl_eval_postfix_exp(JLVALUE *this, JLNode *node)
{
  char *op = node->sval;
  if (STREQ(op, "++"))
    // TODO: postfix increment
    return JLNUM(1);
  if (STREQ(op, "--"))
    // TODO: postfix decrement
    return JLNUM(1);
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
  if (STREQ(op, "++"))
    // TODO: prefix increment
    return JLNUM(1);
  if (STREQ(op, "--"))
    // TODO: prefix decrement
    return JLNUM(1);
}

JLVALUE *
jl_eval_bin_exp(JLVALUE *this, JLNode *node)
{
  JLVALUE *a = jl_eval_exp(this, node->e1);
  JLVALUE *b = jl_eval_exp(this, node->e2);
  char *op = node->sval;

  // Logical
  if (STREQ(op, "&&")) return jl_and(a, b);
  if (STREQ(op, "||")) return jl_or(a, b);

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
  if (STREQ(op, ">=")) return jl_gt(a, b);
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
  if (T_BOTH(a, b, T_NUMBER))
    return JLNUM(a->number.val + b->number.val);

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
  // TODO: Keep it DRY by returning !(a > b || a == b)
  if (T_BOTH(a, b, T_NUMBER)) {
    if (a->number.is_nan || b->number.is_nan) return JLBOOL(0);
    if (a->number.is_inf) return JLBOOL(0);
    if (b->number.is_inf) return JLBOOL(1);
    return a->number.val < b->number.val ? JLBOOL(1) : JLBOOL(0);
  }
  if (T_BOTH(a, b, T_STRING)) {
    // TODO: Lexicographic comparison
  }
}

JLVALUE *
jl_and(JLVALUE *a, JLVALUE *b)
{
  return JLCAST(a, T_BOOLEAN)->boolean.val && JLCAST(b, T_BOOLEAN)->boolean.val ?
    JLBOOL(1) : JLBOOL(0);
}

JLVALUE *
jl_or(JLVALUE *a, JLVALUE *b)
{
  return JLCAST(a, T_BOOLEAN)->boolean.val || JLCAST(b, T_BOOLEAN)->boolean.val ?
    JLBOOL(1) : JLBOOL(0);
}
