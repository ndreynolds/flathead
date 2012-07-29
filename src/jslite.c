// jslite.c
// --------
// Core function implementations

#include <stdio.h>
#include "jslite.h"

JLPROP *
jl_lookup(JLVALUE *obj, char *name) 
{
  JLPROP *prop = NULL;
  HASH_FIND_STR(obj->object.map, name, prop);
  return prop;
}

void 
jl_assign(JLVALUE *obj, char *name, JLVALUE *val)
{
  JLPROP *prop = jl_lookup(obj, name);
  if (prop == NULL) 
    prop = malloc(sizeof(JLPROP));
  prop->name = malloc((strlen(name) + 1) * sizeof(char));
  strcpy(prop->name, name);
  prop->ptr = (JLVALUE *)val;
  HASH_ADD_KEYPTR(hh, obj->object.map, prop->name, strlen(prop->name), prop);
}

void 
jl_assign_natv_func(JLVALUE *obj, char *name, JLNATVFUNC *func_ptr)
{
}

void
jl_gc() 
{
  /**
   * Mark and Sweep
   * --------------
   * Go through the hashtable of JLVariables. Follow them to their values.
   * Mark the values 'keepers'. Sweep away everything else. No mercy.
   */
}

JLVALUE *
jl_alloc_val()
{
  JLVALUE *val = malloc(sizeof(JLVALUE));
  val->type = T_NULL;
  return val;
}

JLVALUE *
jl_new_val(JLTYPE type)
{
  JLVALUE *val = malloc(sizeof(JLVALUE));
  val->type = type;
  return val;
}

JLVALUE *
jl_new_number(double x, bool is_nan, bool is_inf)
{
  JLVALUE *val = jl_new_val(T_NUMBER);
  val->number.val = x;
  val->number.is_nan = is_nan;
  val->number.is_inf = is_inf;
  return val;
}

JLVALUE *
jl_new_string(char *x)
{
  JLVALUE *val = jl_new_val(T_STRING);
  val->string.ptr = malloc((strlen(x) + 1) * sizeof(char));
  strcpy(val->string.ptr, x);
  val->string.len = strlen(x);
  return val;
}

JLVALUE *
jl_new_boolean(bool x)
{
  JLVALUE *val = jl_new_val(T_BOOLEAN);
  val->boolean.val = x;
  return val;
}

JLVALUE *
jl_new_object()
{
  JLVALUE *val = jl_new_val(T_OBJECT);
  JLPROP *map = NULL;
  val->object.map = map;
  return val;
}

JLPROP *
jl_new_prop()
{
}

char *
jl_typeof(JLVALUE *value) 
{
  /* Per Table 20 of the ECMA5 spec: */
  switch(value->type) 
  {
    case T_OBJECT:
      return "object";
    case T_BOOLEAN:
      return "boolean";
    case T_NUMBER:
      return "number";
    case T_STRING:
      return "string";
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

JLVALUE *
jl_cast(JLVALUE *val, JLTYPE type)
{
  // Currently very wasteful. Should repurpose them instead
  // of throwing them out.
  if (val->type == type) return val;
  if (val->type == T_NUMBER && type == T_STRING) {
    if (val->number.is_nan) return JLSTR("NaN");
    if (val->number.is_inf) return JLSTR("Infinity");
    char tmp[100];
    sprintf(tmp, "%g", val->number.val);
    return JLSTR(tmp);
  }
  if (val->type == T_STRING && type == T_NUMBER) {
    char *err;
    double d = strtod(val->string.ptr, &err);
    if (*err != 0) return JLNAN();
    return JLNUM(d);
  }
  if (val->type == T_NULL && type == T_STRING)
    return JLSTR("null");
  if (val->type == T_NULL && type == T_NUMBER)
    return JLNUM(0);
  if (val->type == T_UNDEF && type == T_STRING)
    return JLSTR("undefined");
  if (val->type == T_UNDEF && type == T_NUMBER)
    return JLNAN();
  if (val->type == T_BOOLEAN && type == T_STRING) {
    if (val->boolean.val == 1) return JLSTR("true");
    return JLSTR("false");
  }
  if (val->type == T_BOOLEAN && type == T_NUMBER) {
    if (val->boolean.val == 1) return JLNUM(1);
    return JLNUM(0);
  }
  if (val->type == T_NUMBER && type == T_BOOLEAN) {
    // O is false, x < 0 & x > 0 true
    if (val->number.val == 0) return JLBOOL(0);
    return JLBOOL(1);
  }
  if (val->type == T_STRING && type == T_BOOLEAN) {
    // "" is false, all others true
    if (val->string.len == 0) return JLBOOL(0);
    return JLBOOL(1);
  }
  if (type == T_NULL) return JLNULL();
  if (type == T_UNDEF) return JLUNDEF();
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
