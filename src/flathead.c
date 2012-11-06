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
#include "str.h"
#include "gc.h"


// ----------------------------------------------------------------------------
// Value Constructors
// ----------------------------------------------------------------------------

js_val *
fh_new_val(js_type type)
{
  js_val *val = fh_malloc(true);

  val->map = NULL;
  val->type = type;
  val->signal = S_NONE;
  val->proto = NULL;

  return val;
}

js_val *
fh_new_number(double x, bool is_nan, bool is_inf, bool is_neg)
{
  js_val *val = fh_new_val(T_NUMBER);

  val->number.val = x;
  val->number.is_nan = is_nan;
  val->number.is_inf = is_inf;
  val->number.is_neg = is_neg;
  val->proto = fh_try_get_proto("Number");

  return val;
}

js_val *
fh_new_string(char *x)
{
  js_val *val = fh_new_val(T_STRING);

  val->string.ptr = malloc((strlen(x) + 1) * sizeof(char));
  strcpy(val->string.ptr, x);
  fh_set_len(val, strlen(x));
  val->proto = fh_try_get_proto("String");

  return val;
}

js_val *
fh_new_boolean(bool x)
{
  js_val *val = fh_new_val(T_BOOLEAN);

  val->boolean.val = x;
  val->proto = fh_try_get_proto("Boolean");

  return val;
}

js_val *
fh_new_object()
{
  js_val *val = fh_new_val(T_OBJECT);

  val->object.length = 0;
  val->object.is_array = false;
  val->object.frozen = false;
  val->object.sealed = false;
  val->object.extensible = false;
  val->object.parent = NULL;
  val->object.wraps = NULL;
  val->proto = fh_try_get_proto("Object");

  return val;
}

js_val *
fh_new_array()
{
  js_val *val = fh_new_object();

  val->object.is_array = true;
  val->proto = fh_try_get_proto("Array");
  fh_set_len(val, 0);

  return val;
}

js_val *
fh_new_function(struct ast_node *node)
{
  js_val *val = fh_new_val(T_FUNCTION);

  val->function.is_native = false;
  val->function.is_generator = false;
  val->function.node = node;
  val->function.closure = NULL;
  val->function.instance = NULL;
  val->function.bound_this = NULL;
  val->function.bound_args = NULL;
  val->proto = fh_try_get_proto("Function");

  return val;
}

js_val *
fh_new_native_function(js_native_function func)
{
  js_val *val = fh_new_function(NULL);

  val->function.is_native = true;
  val->function.native = func;

  return val;
}

js_val *
fh_new_regexp(char *re)
{
  js_val *val = fh_new_val(T_OBJECT);

  val->object.is_regexp = true;
  val->proto = fh_try_get_proto("RegExp");

  // Process the trailing options: re = /pattern/[imgy]{0,4}
  int i = strlen(re) - 1;
  while (re[i] != '/' && i > 0) {
    switch(re[i]) {
      case 'g': fh_set(val, "global", JSBOOL(1)); break;
      case 'i': fh_set(val, "ignoreCase", JSBOOL(1)); break;
      case 'm': fh_set(val, "multiline", JSBOOL(1)); break;
      case 'y': fh_set(val, "sticky", JSBOOL(1)); break;
    }
    i--;
  }

  // Store the inner pattern 
  fh_set(val, "source", JSSTR(fh_str_slice(re, 1, i)));

  return val;
}

js_prop *
fh_new_prop(js_prop_flags flags)
{
  js_prop *prop = malloc(sizeof(js_prop));

  prop->writable = flags & P_WRITE;
  prop->configurable = flags & P_CONF;
  prop->enumerable = flags & P_ENUM;
  prop->circular = false;
  prop->ptr = NULL;

  return prop;
}

eval_state *
fh_new_state(int line, int column)
{
  eval_state *state = malloc(sizeof(eval_state));

  state->line = line;
  state->column = column;
  state->ctx = NULL;
  state->this = NULL;
  state->construct = false;

  return state;
}

fh_state *
fh_new_global_state()
{
  fh_state *state = malloc(sizeof(fh_state));

  state->gc_state = GC_STATE_NONE;
  memset(state->gc_arenas, 0, sizeof(state->gc_arenas));
  state->gc_num_arenas = 0;
  state->gc_runs = 0;
  state->gc_time = 0;
  state->gc_last_start = 0;

  state->global = NULL;
  state->function_proto = NULL;
  state->object_proto = NULL;

  state->opt_interactive = false;
  state->opt_print_tokens = false;
  state->opt_print_ast = false;

  return state;
}

js_args *
fh_new_args(js_val *arg1, js_val *arg2, js_val *arg3)
{
  js_args *args = malloc(sizeof(js_args));

  args->arg = arg1;
  args->next = NULL;
  if (arg2) {
    args->next = malloc(sizeof(js_args));
    args->next->arg = arg2;
    args->next->next = NULL;
  }
  if (arg3) {
    args->next->next = malloc(sizeof(js_args));
    args->next->next->arg = arg3;
    args->next->next->next = NULL;
  }

  return args;
}


// ----------------------------------------------------------------------------
// Value Casting
// ----------------------------------------------------------------------------

js_val *
fh_cast(js_val *val, js_type type)
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

  UNREACHABLE();
}


// ----------------------------------------------------------------------------
// Error Handling
// ----------------------------------------------------------------------------

void
fh_error(eval_state *state, js_error_type type, const char *tpl, ...)
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
fh_typeof(js_val *value) 
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

js_val *
fh_has_instance(js_val *func, js_val *val)
{
  if (!IS_FUNC(func))
    fh_error(NULL, E_TYPE, "");
  // TODO: implement
  (void)val;
  return JSBOOL(0);
}

js_val *
fh_has_property(js_val *obj, char *prop)
{
  js_val *val = fh_get_proto(obj, prop);
  return JSBOOL(!IS_UNDEF(val));
}

js_val *
fh_try_get_proto(char *type)
{
  js_val *global = fh->global;
  if (global != NULL) {
    js_val *obj = fh_get(global, type);
    if (!IS_UNDEF(obj))
      return fh_get(obj, "prototype");
  }
  return NULL;
}
  
js_val *
fh_get_arg(js_args *args, int n)
{
  int i;
  for (i = 0; i <= n; i++) {
    if (i == n) 
      return args->arg != NULL ? args->arg : JSUNDEF();
    if (args->next == NULL) 
      break;
    args = args->next;
  }
  return JSUNDEF();
}

int
fh_arg_len(js_args *args)
{
  int i = 0;
  while (true)
  {
    if (args == NULL) return i;
    if (args->arg != NULL) i++;
    if (args->next == NULL) break;
    args = args->next;
  }
  return i;
}

void
fh_set_len(js_val *val, int len)
{
  if (IS_STR(val))
    val->string.length = len;
  if (IS_ARR(val))
    val->object.length = len;
  fh_set_prop(val, "length", JSNUM(len), P_BUILTIN);
}
