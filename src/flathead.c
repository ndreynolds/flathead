/*
 * flathead.c -- Memory management, casting, and debug helpers
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

#include "flathead.h"
#include "gc.h"

/**
 * Lookup a property on an object, resolve the value, and return it.
 */
JSValue *
fh_get(JSValue *obj, char *name)
{
  // We can't read properties from undefined.
  if (obj->type == T_UNDEF)
    fh_error(NULL, E_TYPE, "Cannot read property '%s' of undefined", name);

  JSProp *prop = fh_get_prop(obj, name);
  // But we'll happily return undefined if a property doesn't exist.
  if (prop == NULL) return JSUNDEF();
  return prop->ptr;
}

/**
 * Same as `fh_get`, but recurse the scope chain.
 */
JSValue *
fh_get_rec(JSValue *obj, char *name) 
{
  JSProp *prop = fh_get_prop_rec(obj, name);
  return prop == NULL ?  JSUNDEF() : prop->ptr;
}

/**
 * Same as `fh_get`, but recurse the prototype chain (if one exists).
 */
JSValue *
fh_get_proto(JSValue *obj, char *name)
{
  JSProp *prop = fh_get_prop_proto(obj, name);
  JSValue *val = prop ? prop->ptr : JSUNDEF();
  // Store a ref to the instance for natively define methods.
  if (val->type == T_FUNCTION) {
    val->function.instance = obj;
  }
  return val;
}

/**
 * Lookup a property on an object and return it.
 */
JSProp *
fh_get_prop(JSValue *obj, char *name)
{
  JSProp *prop = NULL;
  if (obj->object.map)
    HASH_FIND_STR(obj->object.map, name, prop);
  return prop;
}

void
fh_set(JSValue *obj, char *name, JSValue *val)
{
  bool add = false;
  JSProp *prop = fh_get_prop(obj, name);
  if (prop == NULL) {
    prop = fh_new_prop(true, true, true);
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

JSProp *
fh_get_prop_rec(JSValue *obj, char *name)
{
  JSProp *prop = fh_get_prop(obj, name);
  // If not found here, check the parent object.
  if (prop == NULL && obj->object.parent != NULL)
    return fh_get_prop_rec(obj->object.parent, name);
  return prop;
}

JSProp *
fh_get_prop_proto(JSValue *obj, char *name)
{
  JSProp *prop = fh_get_prop(obj, name);
  if (prop == NULL && obj->proto != NULL)
    return fh_get_prop_proto(obj->proto, name);
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
fh_set_rec(JSValue *obj, char *name, JSValue *val)
{
  JSValue *scope_to_set = obj;
  JSValue *parent = NULL;

  // Try and find the property in a parent scope.
  JSProp *prop = fh_get_prop(obj, name);
  while(prop == NULL) {
    if (obj->object.parent != NULL) {
      parent = obj->object.parent;
      prop = fh_get_prop(parent, name);
      obj = parent;
      continue;
    }
    break;
  }
  if (prop != NULL && parent != NULL)
    scope_to_set = parent;

  fh_set(scope_to_set, name, val);
}

JSValue *
fh_try_get_proto(char *type)
{
  JSValue *global = fh_global();
  if (global != NULL) {
    JSValue *obj = fh_get(global, type);
    if (obj->type != T_UNDEF)
      return fh_get(obj, "prototype");
  }
  return NULL;
}

JSValue *
fh_new_val(JSType type)
{
  JSValue *val = fh_malloc(true);
  val->type = type;
  return val;
}

JSValue *
fh_new_number(double x, bool is_nan, bool is_inf, bool is_neg)
{
  JSValue *val = fh_new_val(T_NUMBER);
  val->number.val = x;
  val->number.is_nan = is_nan;
  val->number.is_inf = is_inf;
  val->number.is_neg = is_neg;
  val->proto = fh_try_get_proto("Number");
  return val;
}

JSValue *
fh_new_string(char *x)
{
  JSValue *val = fh_new_val(T_STRING);
  val->string.ptr = malloc((strlen(x) + 1) * sizeof(char));
  strcpy(val->string.ptr, x);
  val->string.len = strlen(x);
  //val->object.proto = fh_get(fh_get(fh_global(), "String"), "prototype");
  return val;
}

JSValue *
fh_new_boolean(bool x)
{
  JSValue *val = fh_new_val(T_BOOLEAN);
  val->boolean.val = x;
  //val->object.proto = fh_get(fh_get(fh_global(), "Boolean"), "prototype");
  return val;
}

JSValue *
fh_new_object()
{
  JSValue *val = fh_new_val(T_OBJECT);
  JSProp *map = NULL;
  val->object.map = map;
  val->proto = fh_try_get_proto("Object");
  return val;
}

JSValue *
fh_new_function(struct Node *node)
{
  JSValue *val = fh_new_val(T_FUNCTION);
  val->function.node = node;
  return val;
}

JSValue *
fh_new_native_function(JSNativeFunction func)
{
  JSValue *val = fh_new_val(T_FUNCTION);
  val->function.is_native = true;
  val->function.native = func;
  return val;
}

JSProp *
fh_new_prop(bool writable, bool configurable, bool enumerable)
{
  JSProp *prop = malloc(sizeof(JSProp));

  prop->writable = writable;
  prop->configurable = configurable;
  prop->enumerable = enumerable;

  prop->circular = false;
  prop->ptr = NULL;
  return prop;
}

State *
fh_new_state(int line, int column)
{
  State *state = malloc(sizeof(State));
  state->line = line;
  state->column = column;
  return state;
}

char *
fh_typeof(JSValue *value) 
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
fh_str_concat(char *dst, char *new)
{
  dst = realloc(dst, strlen(dst) + strlen(new) + sizeof(char));
  if (!dst) exit(1);
  strcat(dst, new);
  return dst;
}

JSValue *
fh_cast(JSValue *val, JSType type)
{
  if (val->type == type) return val;
  if (type == T_NULL) return JSNULL();
  if (type == T_UNDEF) return JSUNDEF();

  // Number => x
  if (val->type == T_NUMBER) {
    if (type == T_STRING) {
      if (val->number.is_nan) return JSSTR("NaN");
      if (val->number.is_inf) return JSSTR("Infinity");
      char tmp[100];
      sprintf(tmp, "%g", val->number.val);
      return JSSTR(tmp);
    }
    if (type == T_BOOLEAN) {
      // O is false, x < 0 & x > 0 true
      if (val->number.val == 0) return JSBOOL(0);
      return JSBOOL(1);
    }
  }

  // String => x
  if (val->type == T_STRING) {
    if (type == T_NUMBER) {
      char *err;
      double d = strtod(val->string.ptr, &err);
      if (*err != 0) return JSNAN();
      return JSNUM(d);
    }
    if (type == T_BOOLEAN) {
      // "" is false, all others true
      if (val->string.len == 0) return JSBOOL(0);
      return JSBOOL(1);
    }
  }

  // Boolean => x
  if (val->type == T_BOOLEAN) {
    if (type == T_STRING) {
      if (val->boolean.val == 1) return JSSTR("true");
      return JSSTR("false");
    }
    if (type == T_NUMBER) {
      if (val->boolean.val == 1) return JSNUM(1);
      return JSNUM(0);
    }
  }

  // Object => x
  if (val->type == T_OBJECT) {
    if (type == T_BOOLEAN) return JSBOOL(1);
    if (type == T_NUMBER) {
      if (val->object.is_array) return JSNUM(0);
      return JSNAN();
    }
    // TODO: Call Object.toString()
    if (type == T_STRING) return JSSTR("[Object object]");
  }

  // Function => x
  if (val->type == T_FUNCTION) {
    if (type == T_BOOLEAN) return JSBOOL(1);
    if (type == T_NUMBER) return JSNAN();
    // TODO: Call Function.toString()
    if (type == T_STRING) return JSSTR("[Function]");
  }

  // null => x
  if (val->type == T_NULL) {
    if (type == T_STRING) return JSSTR("null");
    if (type == T_NUMBER) return JSNUM(0);
    if (type == T_BOOLEAN) return JSBOOL(0);
  }

  // undefined => x
  if (val->type == T_UNDEF) {
    if (type == T_STRING) return JSSTR("undefined");
    if (type == T_NUMBER) return JSNAN();
    if (type == T_BOOLEAN) return JSBOOL(0);
  }

  assert(0);
}

void
fh_error(State *state, JSErrorType type, const char *tpl, ...)
{
  char *name = type == E_TYPE ?  "TypeError" : 
               type == E_SYNTAX ? "SyntaxError" :
               type == E_EVAL ? "EvalError" :
               type == E_RANGE ? "RangeError" :
               type == E_ASSERTION ? "AssertionError" :
               type == E_REFERENCE ? "ReferenceError" :
               type == E_PARSE ? "ParseError" :
               "Error";

  fprintf(stderr, "%s: ", name);

  va_list ap;
  va_start(ap, tpl);
  vfprintf(stderr, tpl, ap);
  va_end(ap);
  fprintf(stderr, "\n");

  if (state != NULL)
    fprintf(stderr, "  at Line %d:%d\n", state->line, state->column);
  exit(1);
}

void
fh_debug_obj(FILE *stream, JSValue *obj, int indent)
{
  if (HASH_COUNT(obj->object.map) == 0) {
    fprintf(stream, "{}");
    return;
  }

  JSProp *x;
  int i;
  bool first = true;

  OBJ_ITER(obj, x) {
    if (first) {
      fprintf(stream, "\n");
      for(i=0;i<(indent);i++) fprintf(stream, " ");
      fprintf(stream, "{");
      first = false;
    }
    else {
      fprintf(stream, ",\n");
      for(i=0;i<(indent+1);i++) fprintf(stream, " ");
    }
    fprintf(stream, " %s: ", x->name);
    x->circular ? 
      fprintf(stream, "[Circular]") : 
      fh_debug(stream, x->ptr, indent+3, false);
  };
  fprintf(stream, " }");
}

void
fh_debug_arr(FILE *stream, JSValue *arr, int indent)
{
  if (HASH_COUNT(arr->object.map) == 0) {
    fprintf(stream, "[]");
    return;
  }
  bool first = true;
  JSProp *x, *tmp;
  fprintf(stream, "[ ");
  HASH_ITER(hh, arr->object.map, x, tmp) {
    if (!first) 
      fprintf(stream, ", ");
    else first = false;
    fh_debug(stream, x->ptr, 0, false);
  };
  fprintf(stream, " ]");
}

void
fh_debug(FILE *stream, JSValue *val, int indent, bool newline)
{
  switch(val->type)
  {
    fprintf(stream, "%s", fh_typeof(val));
    case T_BOOLEAN:
      fprintf(stream, "%s", !val->boolean.val ? "false" : "true");
      break;
    case T_NUMBER:
      if (val->number.is_nan)
        fprintf(stream, "NaN");
      else if (val->number.is_inf) 
        fprintf(stream, "%sInfinity", val->number.is_neg ? "-" : "");
      else 
        fprintf(stream, "%g", val->number.val);
      break;
    case T_STRING:
      fprintf(stream, "%s", val->string.ptr);
      break;
    case T_NULL:
      fprintf(stream, "null");
      break;
    case T_FUNCTION:
      fprintf(stream, "[Function]");
      break;
    case T_UNDEF:
      fprintf(stream, "undefined");
      break;
    case T_OBJECT:
      if (val->object.is_array)
        fh_debug_arr(stream, val, indent);
      else
        fh_debug_obj(stream, val, indent);
      break;
  }
  if (newline) fprintf(stream, "\n");
}

void
fh_debug_args(FILE *stream, JSArgs *args)
{
  bool first = true;
  while(first || args->next != NULL)
  {
    if (!first)
      args = args->next;
    if (args->arg != NULL)
      fh_debug(stream, args->arg, 0, 1);
    first = false;
  }
}

JSValue *
fh_get_arg(JSArgs *args, int n)
{
  int i;
  for (i=0; i<=n; i++) {
    if (i == n) 
      return args->arg != NULL ? args->arg : JSNULL();
    if (args->next == NULL) 
      break;
    args = args->next;
  }
  return JSNULL();
}

int
fh_arg_len(JSArgs *args)
{
  int i = 0;
  while(1)
  {
    if (args->arg != NULL) i++;
    if (args->next == NULL) break;
    args = args->next;
  }
  return i;
}
