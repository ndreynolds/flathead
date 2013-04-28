/*
 * flathead.c -- Core types, constructors, casting, and debug.
 *
 * Copyright (c) 2012-2013 Nick Reynolds
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

#include <math.h>

#include "flathead.h"
#include "props.h"
#include "debug.h"
#include "str.h"
#include "gc.h"
#include "eval.h"
#include "args.h"


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

  if (isnan(x)) 
    is_nan = true;
  if (isinf(x)) {
    is_inf = true;
    is_neg = x < 0;
  }

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

  fh_set_class(val, "Object");
  val->object.length = 0;
  val->object.extensible = false;
  val->object.parent = NULL;
  val->object.primitive = NULL;
  val->proto = fh->object_proto;

  return val;
}

js_val *
fh_new_array()
{
  js_val *val = fh_new_object();

  fh_set_class(val, "Array");
  fh_set_len(val, 0);
  val->proto = fh->array_proto;

  return val;
}

js_val *
fh_new_function(struct ast_node *node)
{
  js_val *val = fh_new_object();

  fh_set_class(val, "Function");
  fh_set_prop(val, "prototype", JSOBJ(), P_WRITE);

  fh_set(val, "name", JSSTR(""));
  fh_set(val, "arguments", JSNULL());
  fh_set(val, "caller", JSNULL());

  val->object.native = false;
  val->object.generator = false;
  val->object.node = node;
  val->object.scope = NULL;
  val->object.instance = NULL;
  val->object.bound_this = NULL;
  val->object.bound_args = NULL;
  val->proto = fh->function_proto;

  // Set the function length. Native functions must do this manually.
  fh_set_len(val, (node && node->e1) ? node_count(node->e1) : 0);

  return val;
}

js_val *
fh_new_native_function(js_native_function func, int length)
{
  js_val *val = fh_new_function(NULL);

  val->object.native = true;
  val->object.nativefn = func;
  fh_set_len(val, length);

  return val;
}

js_val *
fh_new_regexp(char *re)
{
  js_val *val = fh_new_val(T_OBJECT);

  val->proto = fh_try_get_proto("RegExp");

  // Process the trailing options: re = /pattern/[imgy]{0,4}
  int i = strlen(re) - 1;
  while (re[i] != '/' && i > 0) {
    switch (re[i]) {
      case 'g': fh_set(val, "global", JSBOOL(1)); break;
      case 'i': fh_set(val, "ignoreCase", JSBOOL(1)); break;
      case 'm': fh_set(val, "multiline", JSBOOL(1)); break;
      case 'y': fh_set(val, "sticky", JSBOOL(1)); break;
    }
    i--;
  }

  // Store the inner pattern 
  if (i > 1)
    fh_set(val, "source", JSSTR(fh_str_slice(re, 1, i)));

  fh_set_class(val, "RegExp");
  return val;
}

js_val *
fh_new_error(char *name, const char *tpl, ...)
{
  js_val *val = fh_new_val(T_OBJECT);
  val->proto = fh_try_get_proto("Error");
  fh_set_class(val, "Error");

  va_list ap;
  va_start(ap, tpl);
  size_t size = vsnprintf(NULL, 0, tpl, ap);
  char *msg = malloc(size + 1);
  vsprintf(msg, tpl, ap);
  va_end(ap);

  fh_set(val, "name", JSSTR(name));
  fh_set(val, "message", JSSTR(msg));
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

void
fh_pop_state()
{
  if (fh->callstack) {
    eval_state *pop = fh->callstack;
    fh->callstack = pop->parent;
    free(pop);
  }
}

void
fh_push_state(eval_state *state)
{
  state->parent = fh->callstack;
  fh->callstack = state;
}

eval_state *
fh_new_state(int line, int column)
{
  eval_state *state = malloc(sizeof(eval_state));
  memset(state, 0, sizeof(eval_state));

  state->line = line;
  state->column = column;
  state->caller_info = NULL;
  state->script_name = fh->script_name;

  state->ctx = NULL;
  state->this = NULL;
  state->parent = NULL;
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

  state->script_name = "main";

  state->opt_interactive = false;
  state->opt_print_tokens = false;
  state->opt_print_ast = false;
  state->opt_keep_history_file = true;
  state->opt_history_filename = ".flathead_history";

  return state;
}


// ----------------------------------------------------------------------------
// Value Checks & Casting
// ----------------------------------------------------------------------------

bool
fh_is_callable(js_val *val)
{
  return IS_FUNC(val);
}

js_val *
fh_to_primitive(js_val *val, js_type hint)
{
  if (!IS_OBJ(val)) return val;

  // This is the [[DefaultValue]] implementation.
  //
  // We'll look for the 'valueOf' and 'toString' props, using the hint to
  // determine that order, and return with the result of calling the first of
  // them that is callable, or a type error otherwise.

  js_val *maybe_func, *res;
  js_args *args;
  char *types[2] = {"valueOf", "toString"};
  int reverse = hint == T_STRING;

  // Without a hint, objects default to Number, except Date 
  if (hint == 0 && IS_DATE(val))
    reverse = true;

  int i; 
  for (i = reverse; i <= 1 && i >= 0; reverse ? i-- : i++) {
    maybe_func = fh_get_proto(val, types[i]);
    if (fh_is_callable(maybe_func)) {
      args = args_new();
      res = fh_call(fh->global, val, maybe_func, args);
      if (!IS_OBJ(res)) return res;
    }
  }
  fh_throw(NULL, fh_new_error(E_TYPE, "cannot convert %s to primitive", fh_typeof(val)));
  UNREACHABLE();
}

js_val *
fh_to_number(js_val *val)
{
  if (IS_UNDEF(val)) return JSNAN();
  if (IS_NULL(val)) return JSNUM(0);
  if (IS_BOOL(val)) {
    if (val->boolean.val == 0) return JSNUM(0);
    return JSNUM(1);
  }
  if (IS_STR(val)) {
    // TODO: check spec
    char *err;
    double d = strtod(val->string.ptr, &err);
    if (*err != 0) return JSNAN();
    return JSNUM(d);
  }
  if (IS_OBJ(val))
    return fh_to_number(fh_to_primitive(val, T_NUMBER));

  return val;
}

js_val *
fh_to_int(js_val *val)
{
  long long int_val;

  val = fh_to_number(val);
  if (IS_NAN(val))
    return JSNUM(0);
  if (IS_INF(val) || val->number.val == 0)
    return val;
  int sign = val->number.val < 0 ? -1 : 1;
  int_val = sign * floor(abs(val->number.val));
  return JSNUM(int_val);
}

js_val *
fh_to_int32(js_val *val)
{
  long long int32_bit = fh_to_uint32(val)->number.val;

  if (int32_bit >= pow(2, 31))
    return JSNUM(int32_bit - pow(2, 32));
  return JSNUM(int32_bit);
}

js_val *
fh_to_uint32(js_val *val)
{
  long long pos_int, int32_bit;

  val = fh_to_number(val);
  if (IS_NAN(val) || IS_INF(val) || val->number.val == 0)
    return JSNUM(0);

  int sign = val->number.val < 0 ? -1 : 1;
  pos_int = sign * floor(abs(val->number.val));
  int32_bit = fmod(pos_int, pow(2, 32));

  return JSNUM(int32_bit);
}

js_val *
fh_to_string(js_val *val)
{
  if (IS_UNDEF(val)) return JSSTR("undefined");
  if (IS_NULL(val)) return JSSTR("null");
  if (IS_BOOL(val)) {
    if (val->boolean.val == 1) return JSSTR("true");
    return JSSTR("false");
  }
  if (IS_NUM(val)) {
    // TODO: check spec
    if (val->number.is_nan) return JSSTR("NaN");
    if (val->number.is_inf) return JSSTR("Infinity");
    char *fmt = "%f";
    if (fmod(val->number.val, 1) == 0) 
      fmt = "%.0f";
    if (fabs(val->number.val) > 1e21)
      fmt = "%g";
    int size = snprintf(NULL, 0, fmt, val->number.val) + 1;
    char *num = malloc(size);
    snprintf(num, size, fmt, val->number.val);
    return JSSTR(num);
  }
  if (IS_OBJ(val))
    return fh_to_string(fh_to_primitive(val, T_STRING));

  return val;
}

js_val *
fh_to_object(js_val *val)
{
  if (IS_UNDEF(val) || IS_NULL(val))
    fh_throw(NULL, fh_new_error(E_TYPE, "cannot convert %s to object", fh_typeof(val)));
  if (IS_OBJ(val)) 
    return val;

  js_val *obj = JSOBJ();
  obj->object.primitive = val;
  if (IS_BOOL(val))
    fh_set_class(obj, "Boolean");
  if (IS_NUM(val))
    fh_set_class(obj, "Number");
  if (IS_STR(val))
    fh_set_class(obj, "String");
  return obj;
}

js_val *
fh_to_boolean(js_val *val)
{
  if (IS_UNDEF(val) || IS_NULL(val)) 
    return JSBOOL(0);
  if (IS_NUM(val))
    return JSBOOL(!IS_NAN(val) && val->number.val != 0);
  if (IS_STR(val))
    return JSBOOL(val->string.length != 0);
  if (IS_OBJ(val))
    return JSBOOL(1);
  return val;
}

js_val *
fh_cast(js_val *val, js_type type)
{
  if (val->type == type) return val;

  switch (type) {
    case T_NULL: return JSNULL();
    case T_UNDEF: return JSUNDEF();
    case T_STRING: return fh_to_string(val);
    case T_NUMBER: return fh_to_number(val);
    case T_BOOLEAN: return fh_to_boolean(val);
    default: return fh_to_object(val);
  }
}


// ----------------------------------------------------------------------------
// Error Handling
// ----------------------------------------------------------------------------

void
fh_throw(eval_state *state, js_val *error)
{
  fprintf(stderr, "%s\n", TO_STR(fh_to_primitive(error, T_STRING))->string.ptr);

  while (state != NULL) {
    if (state->caller_info)
      fprintf(stderr, "  at %s in %s:%u:%u\n", 
          state->caller_info, state->script_name, state->line, state->column);
    else
      fprintf(stderr, "  at %s:%u:%u\n", 
          state->script_name, state->line, state->column);
    if (!state->parent) break;
    state = state->parent;
  }

  if (fh->opt_interactive) {
    fh->callstack = NULL;
    longjmp(fh->jmpbuf, 1); 
  }
  exit(1);
}


// ----------------------------------------------------------------------------
// Utilities
// ----------------------------------------------------------------------------

char *
fh_typeof(js_val *value) 
{
  /* Per Table 20 of the ECMA5 spec: */
  switch (value->type) {
    case T_OBJECT:
    case T_NULL:
      return IS_FUNC(value) ? "function" : "object";
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
    fh_throw(NULL, fh_new_error(E_TYPE, "%s is not a function", fh_typeof(func)));
  js_val *fproto = fh_get(func, "prototype");
  if (!IS_UNDEF(fproto) && IS_OBJ(val)) {
    js_val *proto = val->proto;
    while (proto) {
      if (proto == fproto)
        return JSBOOL(1);
      proto = proto->proto;
    }
  }
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
  
void
fh_set_len(js_val *val, unsigned long len)
{
  if (IS_STR(val))
    val->string.length = len;
  if (IS_ARR(val))
    val->object.length = len;
  fh_set_prop(val, "length", JSNUM(len), 0);
}

void
fh_set_class(js_val *obj, char *class)
{
  strcpy(obj->object.class, class);
}
