/*
 * runtime.c -- Bootstrap global object and friends 
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

#include <math.h>
#include "runtime.h"
#include "lib/console.h"
#include "lib/Math.h"
#include "lib/Object.h"
#include "lib/Function.h"
#include "lib/Array.h"
#include "lib/String.h"
#include "lib/Number.h"
#include "lib/Boolean.h"
#include "lib/Date.h"
#include "lib/RegExp.h"


// isNaN(value)
JSValue *
global_is_nan(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *num = TO_NUM(ARG(args, 0));
  return JSBOOL(num->number.is_nan);
}

// isFinite(number)
JSValue *
global_is_finite(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *num = TO_NUM(ARG(args, 0));
  return JSBOOL(!(num->number.is_nan || num->number.is_inf));
}

// parseInt(string[, radix])
JSValue *
global_parse_int(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *to_parse = TO_STR(ARG(args, 0));
  JSValue *radix_arg = ARG(args, 1);
  
  // FIXME: determine radix based on start of string
  int radix = IS_NUM(radix_arg) ? radix_arg->number.val : 10;
  char *ep;
  long l;

  l = strtol(to_parse->string.ptr, &ep, radix);
  if (*ep != 0) {
    JSValue *num = TO_NUM(to_parse);
    return IS_NAN(num) ? JSNAN() : JSNUM(floor(num->number.val));
  }

  return JSNUM((int)l);
}

// parseFloat(string)
JSValue *
global_parse_float(JSValue *instance, JSArgs *args, State *state)
{
  return TO_NUM(ARG(args, 0));
}

// eval(string)
JSValue *
global_eval(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// gc()
JSValue *
global_gc(JSValue *instance, JSArgs *args, State *state)
{
  fh_gc();
  return JSUNDEF();
}

// load(filename)
//
// Custom Flathead function. Execute the given file in the 
// global scope. Parsing and evaluation occur at runtime.
JSValue *
global_load(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *name = TO_STR(ARG(args, 0));

  FILE *file = fopen(name->string.ptr, "r");
  fh_eval_file(file, fh_global(), false);

  return JSUNDEF();
}

void
fh_connect_prototypes(JSValue *global)
{
  // Hook up the Function.prototype to the Object.prototype methods.
  //
  // Note: this doesn't happen automatically because the Function object
  // does not exist when the Object# methods are defined.
  JSValue *obj_proto = fh_get(fh_get(global, "Object"), "prototype");
  JSValue *func_proto = fh_get(fh_get(global, "Function"), "prototype");
  JSProp *prop;
  OBJ_ITER(obj_proto, prop) {
    if (prop->ptr && IS_FUNC(prop->ptr) && prop->ptr->function.is_native)
      prop->ptr->proto = func_proto;
  }
}

JSValue *
fh_bootstrap()
{
  JSValue *global = JSOBJ();

  fh_gc_register_global(global);

  fh_set(global, "Object", bootstrap_object());
  fh_set(global, "Function", bootstrap_function());
  fh_set(global, "Array", bootstrap_array());
  fh_set(global, "String", bootstrap_string());
  fh_set(global, "Number", bootstrap_number());
  fh_set(global, "Boolean", bootstrap_boolean());
  fh_set(global, "Date", bootstrap_date());
  fh_set(global, "RegExp", bootstrap_regexp());

  fh_connect_prototypes(global);

  fh_set(global, "console", bootstrap_console());
  fh_set(global, "Math", bootstrap_math());
  fh_set(global, "NaN", JSNAN());
  fh_set(global, "Infinity", JSINF());
  fh_set(global, "isNaN", JSNFUNC(&global_is_nan));
  fh_set(global, "isFinite", JSNFUNC(&global_is_finite));
  fh_set(global, "parseInt", JSNFUNC(&global_parse_int));
  fh_set(global, "parseFloat", JSNFUNC(&global_parse_float));
  fh_set(global, "eval", JSNFUNC(&global_eval));
  fh_set(global, "load", JSNFUNC(&global_load));
  fh_set(global, "undefined", JSUNDEF());
  fh_set(global, "this", global);

#ifdef fh_gc_expose
  fh_set(global, "gc", JSNFUNC(&global_gc));
#endif

  return global;
}
