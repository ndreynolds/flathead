/*
 * jslite.c -- Memory management, casting, and debug helpers
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

#include <stdio.h>
#include "jslite.h"

JLPROP *
jl_lookup(JLVALUE *obj, char *name, bool rec) 
{
  JLPROP *prop = NULL;
  HASH_FIND_STR(obj->object.map, name, prop);
  // If not found here, check the parent object.
  if (rec && prop == NULL) {
    HASH_FIND_STR(obj->object.map, "this", prop);
    if (prop != NULL && !prop->circular) {
      JLVALUE *parent = (JLVALUE *)prop->ptr;
      return jl_lookup(parent, name, rec);
    }
  }
  return prop;
}

void 
jl_assign(JLVALUE *obj, char *name, JLVALUE *val)
{
  // TODO: Handle undeclared assignment properly; set prop flags
  JLPROP *prop = jl_lookup(obj, name, false);
  if (prop == NULL) 
    prop = malloc(sizeof(JLPROP));
  prop->name = malloc((strlen(name) + 1) * sizeof(char));
  strcpy(prop->name, name);
  prop->ptr = (JLVALUE *)val;
  // Do we have a circular reference?
  prop->circular = prop->ptr == obj ? 1 : 0;
  HASH_ADD_KEYPTR(hh, obj->object.map, prop->name, strlen(prop->name), prop);
}

void
jl_gc() 
{
  // TODO: Mark and Sweep
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

JLVALUE *
jl_new_function(void *body)
{
  JLVALUE *val = jl_new_val(T_FUNCTION);
  val->function.body = body;
  return val;
}

JLVALUE *
jl_new_native_function(JLNATVFUNC func)
{
  JLVALUE *val = jl_new_val(T_FUNCTION);
  val->function.is_native = true;
  val->function.native = func;
  return val;
}

char *
jl_typeof(JLVALUE *value) 
{
  /* Per Table 20 of the ECMA5 spec: */
  switch(value->type) 
  {
    case T_OBJECT:
    case T_NULL:
      return "object";
    case T_FUNCTION:
      return "function";
    case T_BOOLEAN:
      return "boolean";
    case T_NUMBER:
      return "number";
    case T_STRING:
      return "string";
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
jl_debug_obj(JLVALUE *obj, int indent)
{
  if (HASH_COUNT(obj->object.map) == 0) {
    printf("{}");
    return;
  }

  JLPROP *x, *tmp;
  int i;
  bool first = true;

  HASH_ITER(hh, obj->object.map, x, tmp) {
    if (first) {
      printf("\n");
      for(i=0;i<(indent);i++) printf(" ");
      printf("{");
      first = false;
    }
    else {
      printf(",\n");
      for(i=0;i<(indent+1);i++) printf(" ");
    }

    printf(" %s: ", x->name);
    x->circular ? 
      printf("[Circular]") : 
      jl_debug((JLVALUE *)x->ptr, indent+3, false);
  };
  printf(" }");
}

void
jl_debug(JLVALUE *val, int indent, bool newline)
{
  switch(val->type)
  {
    printf("%s", jl_typeof(val));
    case T_BOOLEAN:
      printf("%s", !val->boolean.val ? "false" : "true");
      break;
    case T_NUMBER:
      if (val->number.is_nan) 
        printf("NaN");
      else if (val->number.is_inf) 
        printf("Infinity");
      else 
        printf("%f", val->number.val);
      break;
    case T_STRING:
      printf("%s", val->string.ptr);
      break;
    case T_NULL:
      printf("null");
    case T_FUNCTION:
      printf("[Function]");
      break;
    case T_UNDEF:
      printf("undefined");
      break;
    case T_OBJECT:
      jl_debug_obj(val, indent);
      break;
  }
  if (newline) printf("\n");
}

void
jl_debug_args(JLARGS *args)
{
  bool first = true;
  while(first || args->next != 0)
  {
    if (!first)
      args = args->next;
    if (args->arg != 0)
      JLDEBUG(args->arg);
    first = false;
  }
}
