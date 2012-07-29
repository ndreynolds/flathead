// eval.c
// ------

#include "eval.h"
#include "nodes.h"
#include <math.h>

#ifndef JLDEBUG
#define JLDEBUG 0
#endif

int 
jl_eval(JLVALUE *this, JLNode *node)
{
  switch(node->type) {
    case NODE_STMT_LST:
      jl_eval(this, node->e1);
      if (node->e2 != 0) jl_eval(this, node->e2);
      break;
    case NODE_EXP_STMT:
      JLDEBUG ?
        jl_debug_value(jl_eval_exp(this, node->e1)) :
        jl_eval_exp(this, node->e1);
      break;
    case NODE_IF:
      // If condition is satisfied, evaluate the ifBlock.
      if (JLCAST(jl_eval_exp(this, node->e1), T_BOOLEAN)->boolean.val)
        jl_eval(this, node->e2);
      // Otherwise, provided there is an elseBlock, evaluate it.
      else if (node->e3 != 0) 
        jl_eval(this, node->e3);
      break;
    case NODE_VAR_STMT:
      // var x;
      jl_assign(this, node->e1->sval, JLNULL());
      break;
    case NODE_WHILE:
      jl_while(this, node->e1, node->e2);
      break;
    case NODE_EMPT_STMT:
      break;
    default:
      fprintf(stderr, "Evaluation error: unsupported structure (%d)\n", node->type);
      return 1;
  }
  return 0;
}

void
jl_while(JLVALUE *this, JLNode *cnd, JLNode *block)
{
  int loop_cnt;
  while(JLCAST(jl_eval_exp(this, cnd), T_BOOLEAN)) {
    jl_eval(JLOBJ(), block);
    loop_cnt++;
  }
}

JLVALUE *
jl_eval_exp(JLVALUE *this, JLNode *node)
{
  if (node->type == NODE_BOOL) return JLBOOL((bool)node->val);
  if (node->type == NODE_STR) return JLSTR(node->sval);
  if (node->type == NODE_NUM) return JLNUM(node->val);
  if (node->type == NODE_NULL) return JLNULL();
  if (node->type == NODE_OBJ) return JLOBJ();
  if (node->type == NODE_ASGN) {
    jl_assign(
        this, 
        JLCAST(jl_eval_exp(this, node->e1), T_STRING)->string.ptr, 
        jl_eval_exp(this, node->e2)
    );
    return jl_eval_exp(this, node->e1);
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
      JLBOOL(0): JLBOOL(1);
  // TODO: prefix inc/dec
}

JLVALUE *
jl_eval_bin_exp(JLVALUE *this, JLNode *node)
{
  char *op = node->sval;
  if (STREQ(op, "+"))
    return jl_add(jl_eval_exp(this, node->e1), jl_eval_exp(this, node->e2));
  if (STREQ(op, "-"))
    return jl_sub(jl_eval_exp(this, node->e1), jl_eval_exp(this, node->e2));
  if (STREQ(op, "*"))
    return jl_mul(jl_eval_exp(this, node->e1), jl_eval_exp(this, node->e2)); 
  if (STREQ(op, "/"))
    return jl_div(jl_eval_exp(this, node->e1), jl_eval_exp(this, node->e2));
  if (STREQ(op, "%"))
    return jl_mod(jl_eval_exp(this, node->e1), jl_eval_exp(this, node->e2));
}

#define T_BOTH(a,b,t)      (a->type == t && b->type == t)
#define T_XOR(a,b,t1,t2)   (a->type == t1 && b->type == t2 || a->type == t2 && b->type == t1)

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
  // We'll be back at the top with numbers: finite, infinite and NaN.
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
