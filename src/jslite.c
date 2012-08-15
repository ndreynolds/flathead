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

JLVALUE *
jl_get(JLVALUE *obj, char *prop_name)
{
  // We can't read properties from undefined.
  if (obj->type == T_UNDEF) {
    fprintf(stderr, "TypeError: Cannot read property '%s' of undefined\n", prop_name);
    exit(1);
  }
  JLPROP *prop = jl_get_prop(obj, prop_name);
  // But we'll happily return undefined if a property doesn't exist.
  if (prop == NULL) return JLUNDEF();
  return prop->ptr;
}

JLPROP *
jl_get_prop(JLVALUE *obj, char *name)
{
  //printf("GET PROP: %s\n", name);
  JLPROP *prop = NULL;
  HASH_FIND_STR(obj->object.map, name, prop);
  return prop;
}

void
jl_set(JLVALUE *obj, char *name, JLVALUE *val)
{
  // TODO: Handle undeclared assignment properly; set prop flags
  bool add = false;
  JLPROP *prop = jl_get_prop(obj, name);
  if (prop == NULL) {
    prop = malloc(sizeof(JLPROP));
    add = true;
  }
  prop->name = malloc((strlen(name) + 1) * sizeof(char));
  strcpy(prop->name, name);
  prop->ptr = val;
  // Do we have a circular reference?
  prop->circular = prop->ptr == obj ? 1 : 0;
  // Don't add if it already exists (bad things happen).
  if (add)
    HASH_ADD_KEYPTR(hh, obj->object.map, prop->name, strlen(prop->name), prop);
}

/*
 * Look for a property on an object and return its value, 
 * checking parent scopes.
 */
JLVALUE *
jl_get_rec(JLVALUE *obj, char *name) 
{
  JLPROP *prop = jl_get_prop_rec(obj, name);
  return prop == NULL ?  JLUNDEF() : (JLVALUE *)prop->ptr;
}

JLPROP *
jl_get_prop_rec(JLVALUE *obj, char *name)
{
  JLPROP *prop = jl_get_prop(obj, name);
  // If not found here, check the parent object.
  if (prop == NULL && obj->object.parent != NULL)
    return jl_get_prop_rec(obj->object.parent, name);
  return prop;
}

/*
 * (Re)set a property on an object, setting the property on the
 * given object, or the closest parent scope on which it is 
 * already defined.
 *
 * This is used in undeclared assignment.
 */
void
jl_set_rec(JLVALUE *obj, char *name, JLVALUE *val)
{
  JLVALUE *scope_to_set = obj;
  JLVALUE *parent;

  // Try and find the property in a parent scope.
  JLPROP *prop = jl_get_prop(obj, name);
  while(prop == NULL) {
    if (obj->object.parent != NULL) {
      parent = (JLVALUE *)obj->object.parent;
      prop = jl_get_prop(parent, name);
      obj = parent;
      continue;
    }
    break;
  }
  if (prop != NULL && parent != NULL)
    scope_to_set = parent;

  jl_set(scope_to_set, name, val);
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
jl_new_function(void *node)
{
  JLVALUE *val = jl_new_val(T_FUNCTION);
  val->function.node = node;
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
  if (type == T_NULL) return JLNULL();
  if (type == T_UNDEF) return JLUNDEF();

  // Number => x
  if (val->type == T_NUMBER) {
    if (type == T_STRING) {
      if (val->number.is_nan) return JLSTR("NaN");
      if (val->number.is_inf) return JLSTR("Infinity");
      char tmp[100];
      sprintf(tmp, "%g", val->number.val);
      return JLSTR(tmp);
    }
    if (type == T_BOOLEAN) {
      // O is false, x < 0 & x > 0 true
      if (val->number.val == 0) return JLBOOL(0);
      return JLBOOL(1);
    }
  }

  // String => x
  if (val->type == T_STRING) {
    if (type == T_NUMBER) {
      char *err;
      double d = strtod(val->string.ptr, &err);
      if (*err != 0) return JLNAN();
      return JLNUM(d);
    }
    if (type == T_BOOLEAN) {
      // "" is false, all others true
      if (val->string.len == 0) return JLBOOL(0);
      return JLBOOL(1);
    }
  }

  // Boolean => x
  if (val->type == T_BOOLEAN) {
    if (type == T_STRING) {
      if (val->boolean.val == 1) return JLSTR("true");
      return JLSTR("false");
    }
    if (type == T_NUMBER) {
      if (val->boolean.val == 1) return JLNUM(1);
      return JLNUM(0);
    }
  }

  // Object => x
  if (val->type == T_OBJECT) {
    if (type == T_BOOLEAN) return JLBOOL(1);
    if (type == T_NUMBER) return JLNAN();
    // TODO: Call Object.toString()
    if (type == T_STRING) return JLSTR("[Object object]");
  }

  // Function => x
  if (val->type == T_FUNCTION) {
    if (type == T_BOOLEAN) return JLBOOL(1);
    if (type == T_NUMBER) return JLNAN();
    // TODO: Call Function.toString()
    if (type == T_STRING) return JLSTR("[Function]");
  }

  // null => x
  if (val->type == T_NULL) {
    if (type == T_STRING) return JLSTR("null");
    if (type == T_NUMBER) return JLNUM(0);
  }

  // undefined => x
  if (val->type == T_UNDEF) {
    if (type == T_STRING) return JLSTR("undefined");
    if (type == T_NUMBER) return JLNAN();
  }
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
jl_debug_arr(JLVALUE *arr, int indent)
{
  if (HASH_COUNT(arr->object.map) == 0) {
    printf("[]");
    return;
  }
  bool first;
  JLPROP *x, *tmp;
  printf("[ ");
  HASH_ITER(hh, arr->object.map, x, tmp) {
    if (!first) 
      printf(", ");
    else first = false;
    jl_debug((JLVALUE *)x->ptr, 0, false);
  };
  printf(" ]");
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
        printf("%g", val->number.val);
      break;
    case T_STRING:
      printf("%s", val->string.ptr);
      break;
    case T_NULL:
      printf("null");
      break;
    case T_FUNCTION:
      printf("[Function]");
      break;
    case T_UNDEF:
      printf("undefined");
      break;
    case T_OBJECT:
      if (val->object.is_array)
        jl_debug_arr(val, indent);
      else
        jl_debug_obj(val, indent);
      break;
  }
  if (newline) printf("\n");
}

void
jl_debug_args(JLARGS *args)
{
  bool first = true;
  while(first || args->next != NULL)
  {
    if (!first)
      args = args->next;
    if (args->arg != NULL)
      JLDEBUG(args->arg);
    first = false;
  }
}
