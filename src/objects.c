// objects.c

#include <stdio.h>
#include "jslite.h"

struct JLVariable *scope = NULL;

struct JLVariable *
lookup(char *name) 
{
  struct JLVariable *var;
  HASH_FIND_STR(scope, name, var);
  return var;
}

void 
assign(char *name, JLVALUE *ptr) 
{
  struct JLVariable *var = lookup(name);
  if (var == NULL) var = malloc(sizeof(struct JLVariable));
  var->name = name;
  var->ptr = ptr;
  HASH_ADD_KEYPTR(hh, scope, var->name, strlen(var->name), var);
}

void
gc() {
  /**
   * Mark and Sweep
   * --------------
   * Go through the hashtable of JLVariables. Follow them to their values.
   * Mark the values 'keepers'. Sweep away everything else. No mercy.
   */
}

JLVALUE *
alloc_val()
{
  JLVALUE *val = malloc(sizeof(JLVALUE));
  val->type = T_NULL;
  return val;
}

JLVALUE *
new_number(double x)
{
  JLVALUE *val = alloc_val();
  val->number.val = x;
  val->type = T_NUMBER;
  return val;
}

JLVALUE *
new_string(char *x)
{
  JLVALUE *val = alloc_val();
  val->string.ptr = x;
  val->type = T_STRING;
  return val;
}

JLVALUE *
new_boolean(bool x)
{
  JLVALUE *val = alloc_val();
  val->type = T_BOOLEAN;
  val->boolean.val = x;
  return val;
}

JLVALUE *
new_null()
{
  JLVALUE *val = alloc_val();
  val->type = T_NULL;
  return val;
}

char *
jl_typeof(JLVALUE *value) 
{
  /* Per Table 20 of the ECMA5 spec: */
  switch(value->type) 
  {
    case T_BOOLEAN:
      return "boolean";
    case T_NUMBER:
      return "number";
    case T_STRING:
      return "string";
    case T_ARRAY:
      return "array";
    case T_NULL:
      return "object";
    default:
      return "undefined";
  }
}

char *
jl_str_concat(char *dst, char *new)
{
  dst = realloc(dst, strlen(dst) + strlen(new) + sizeof(char));
  if (!dst) exit(1);
  strcat(dst, new);
  return dst;
}

void
jl_debug_value(JLVALUE *val)
{
  printf("%s ", jl_typeof(val));
  switch(val->type) 
  {
    case T_BOOLEAN:
      printf("%s", !val->boolean.val ? "false" : "true");
      break;
    case T_NUMBER:
      printf("%f", val->number.val);
      break;
    case T_STRING:
      printf("%s", val->string.ptr);
      break;
    default:
      break;
  }
  printf("\n");
}
