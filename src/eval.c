// eval.c
// ------

#include "eval.h"
#include "nodes.h"

int 
jl_eval(JLNode *node, bool debug)
{
  switch(node->type) {
    case NODE_STMT_LST:
      jl_eval(node->e1, debug);
      if (node->e2 != 0) jl_eval(node->e2, debug);
      break;
    case NODE_EXP_STMT:
      debug ?  
        jl_debug_value(jl_eval_exp(node->e1)) :
        jl_eval_exp(node->e1);
      break;
    default:
      fprintf(stderr, "Unknown");
      return 1;
  }
  return 0;
}

JLVALUE *
jl_eval_exp(JLNode *node)
{
  if (node->type == NODE_BOOL)
    return JLBOOL((bool)node->val);
  if (node->type == NODE_STR)
    return JLSTR(node->sval);
  if (node->type == NODE_NUM)
    return JLNUM(node->val);
  if (node->type == NODE_NULL)
    return JLNULL();
  if (node->type == NODE_EXP) {
    switch(node->sub_type) {
      case NODE_UNARY_POST:
        return jl_eval_postfix_exp(node);
      case NODE_UNARY_PRE:
        return jl_eval_prefix_exp(node);
      default:
        return jl_eval_bin_exp(node);
    }
  }
}

JLVALUE *
jl_eval_postfix_exp(JLNode *node)
{
  char *op = node->sval;
  if (strcmp(op, "++") == 0)
    // TODO: increment
    return JLNUM(1);
  if (strcmp(op, "--") == 0)
    // TODO: decrement
    return JLNUM(1);
}

JLVALUE *
jl_eval_prefix_exp(JLNode *node)
{
  char *op = node->sval;
  if (strcmp(op, "+") == 0)
    // TODO: cast to number (or NaN)
    return JLNUM(1);
  if (strcmp(op, "-") == 0)
    // TODO: cast to number (or NaN) and negate
    return JLNUM(1);
  if (strcmp(op, "!") == 0)
    // TODO: cast to boolean and negate
    return JLBOOL(0);
}

JLVALUE *
jl_eval_bin_exp(JLNode *node)
{
  char *op = node->sval;
  if (strcmp(op, "+") == 0)
    return jl_add(jl_eval_exp(node->e1), jl_eval_exp(node->e2));
  if (strcmp(op, "-") == 0)
    return jl_sub(jl_eval_exp(node->e1), jl_eval_exp(node->e2));
  if (strcmp(op, "*") == 0)
    return jl_mul(jl_eval_exp(node->e1), jl_eval_exp(node->e2)); 
}

JLVALUE *
jl_add(JLVALUE *a, JLVALUE *b)
{
  JLVALUE *c;

  // Treat bools as numbers (0 & 1)
  if (a->type == T_BOOLEAN) a->type = T_NUMBER;
  if (b->type == T_BOOLEAN) b->type = T_NUMBER;

  // Two strings => String
  if (a->type == T_STRING && b->type == T_STRING)
    c = JLSTR(jl_str_concat(a->string.ptr, b->string.ptr));

  // Two numbers => Number
  else if (a->type == T_NUMBER && b->type == T_NUMBER)
    c = JLNUM(a->number.val + b->number.val);

  // String and a number => String
  else if (a->type == T_STRING && b->type == T_NUMBER ||
           a->type == T_NUMBER && b->type == T_STRING)
  {
    // something complicated
    c = JLSTR(a->string.ptr);
  }

  // String and null => String
  else if (a->type == T_STRING && b->type == T_NULL ||
           a->type == T_NULL && b->type == T_STRING)
  {
    c = JLSTR(a->type == T_NULL ? 
              jl_str_concat("null", b->string.ptr) : 
              jl_str_concat(a->string.ptr, "null"));
  }

  // Number and null => Null
  else if (a->type == T_NUMBER && b->type == T_NULL ||
           a->type == T_NULL && b->type == T_NUMBER)
    c = JLNUM(a->type == T_NULL ? b->number.val : c->number.val);

  // TODO: Number and undefined => NaN
  // TODO: String and undefined => String
  
  return c;
}

JLVALUE *
jl_sub(JLVALUE *a, JLVALUE *b)
{
  JLVALUE *c;

  // Treat bools as numbers (0 & 1)
  if (a->type == T_BOOLEAN) a->type = T_NUMBER;
  if (b->type == T_BOOLEAN) b->type = T_NUMBER;

  // Parse string operands
  // TODO

  // Two numbers => Number
  else if (a->type == T_NUMBER && b->type == T_NUMBER)
    c = JLNUM(a->number.val - b->number.val);

  // NaN and a number => NaN
  else if (a->type == T_STRING && b->type == T_NUMBER ||
           a->type == T_NUMBER && b->type == T_STRING)
  {
    // something complicated
    c = JLSTR(a->string.ptr);
  }

  // NaN and null => String
  else if (a->type == T_STRING && b->type == T_NULL ||
           a->type == T_NULL && b->type == T_STRING)
  {
    c = JLSTR(a->type == T_NULL ? 
              jl_str_concat("null", b->string.ptr) : 
              jl_str_concat(a->string.ptr, "null"));
  }

  // Number and null => Null
  else if (a->type == T_NUMBER && b->type == T_NULL ||
           a->type == T_NULL && b->type == T_NUMBER)
    c = JLNUM(a->type == T_NULL ? b->number.val : c->number.val);

  // TODO: Number and undefined => NaN
  // TODO: String and undefined => String
  
  return c;
}

JLVALUE *
jl_mul(JLVALUE *a, JLVALUE *b)
{
  // TODO
  return JLNUM(1);
}
