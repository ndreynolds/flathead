/*
 * flathead.c -- Core types, constructors, casting, and debug.
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
#include "props.h"
#include "debug.h"
#include "gc.h"


// ----------------------------------------------------------------------------
// Value Constructors
// ----------------------------------------------------------------------------

JSValue *
fh_new_val(JSType type)
{
  JSValue *val = fh_malloc(true);
  val->type = type;
  val->signal = S_NONE;
  val->proto = NULL;
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
  val->string.length = strlen(x);
  val->proto = fh_try_get_proto("String");
  return val;
}

JSValue *
fh_new_boolean(bool x)
{
  JSValue *val = fh_new_val(T_BOOLEAN);
  val->boolean.val = x;
  val->proto = fh_try_get_proto("Boolean");
  return val;
}

JSValue *
fh_new_object()
{
  JSValue *val = fh_new_val(T_OBJECT);

  val->object.map = NULL;

  val->object.length = 0;
  val->object.is_array = false;
  val->object.frozen = false;
  val->object.sealed = false;
  val->object.extensible = false;

  val->proto = fh_try_get_proto("Object");
  return val;
}

JSValue *
fh_new_array()
{
  JSValue *arr = fh_new_object();

  arr->object.is_array = true;
  arr->proto = fh_try_get_proto("Array");

  fh_arr_set_len(arr, 0);
  return arr;
}

JSValue *
fh_new_function(struct Node *node)
{
  JSValue *val = fh_new_val(T_FUNCTION);

  val->function.is_native = false;
  val->function.is_generator = false;
  val->function.node = node;
  val->function.closure = NULL;
  val->function.instance = NULL;
  val->proto = fh_try_get_proto("Function");

  return val;
}

JSValue *
fh_new_native_function(JSNativeFunction func)
{
  JSValue *val = fh_new_val(T_FUNCTION);

  val->function.is_native = true;
  val->function.is_generator = false;
  val->function.native = func;
  val->function.instance = NULL;

  return val;
}

JSProp *
fh_new_prop(JSPropFlags flags)
{
  JSProp *prop = malloc(sizeof(JSProp));

  prop->writable = flags & P_WRITE;
  prop->configurable = flags & P_CONF;
  prop->enumerable = flags & P_ENUM;

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

JSArgs *
fh_new_args(JSValue *arg1, JSValue *arg2, JSValue *arg3)
{
  JSArgs *args = malloc(sizeof(JSArgs));

  args->arg = arg1;
  args->next = NULL;
  if (arg2) {
    args->next = malloc(sizeof(JSArgs));
    args->next->arg = arg2;
    args->next->next = NULL;
  }
  if (arg3) {
    args->next->next = malloc(sizeof(JSArgs));
    args->next->next->arg = arg3;
    args->next->next->next = NULL;
  }

  return args;
}


// ----------------------------------------------------------------------------
// Value Casting
// ----------------------------------------------------------------------------

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
      int size = snprintf(NULL, 0, "%g", val->number.val) + 1;
      char *num = malloc(size);
      snprintf(num, size, "%g", val->number.val);
      return JSSTR(num);
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
      if (val->string.length == 0) return JSBOOL(0);
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


// ----------------------------------------------------------------------------
// Error Handling
// ----------------------------------------------------------------------------

void
fh_error(State *state, JSErrorType type, const char *tpl, ...)
{
  char *name = type == E_TYPE ? "TypeError" : 
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


// ----------------------------------------------------------------------------
// Utilities
// ----------------------------------------------------------------------------

char *
fh_typeof(JSValue *value) 
{
  /* Per Table 20 of the ECMA5 spec: */
  switch(value->type) {
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

JSValue *
fh_has_instance(JSValue *func, JSValue *val)
{
  if (!IS_FUNC(func))
    fh_error(NULL, E_TYPE, "");
  // TODO: implement
  return JSBOOL(0);
}

JSValue *
fh_has_property(JSValue *obj, char *prop)
{
  JSValue *val = fh_get_proto(obj, prop);
  return JSBOOL(val->type != T_UNDEF);
}

JSValue *
fh_try_get_proto(char *type)
{
  JSValue *global = fh_global();
  if (global != NULL) {
    JSValue *obj = fh_get(global, type);
    if (!IS_UNDEF(obj))
      return fh_get(obj, "prototype");
  }
  return NULL;
}

char *
fh_str_concat(char *a, char *b)
{
  size_t size = strlen(a) + strlen(b) + 1;
  char *new = malloc(size);
  snprintf(new, size, "%s%s", a, b);
  return new;
}
  
JSValue *
fh_get_arg(JSArgs *args, int n)
{
  int i;
  for (i=0; i<=n; i++) {
    if (i == n) 
      return args->arg != NULL ? args->arg : JSUNDEF();
    if (args->next == NULL) 
      break;
    args = args->next;
  }
  return JSUNDEF();
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

void
fh_arr_set_len(JSValue *arr, int len)
{
  arr->object.length = len;
  fh_set_prop(arr, "length", JSNUM(len), P_BUILTIN);
}
