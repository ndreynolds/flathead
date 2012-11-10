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
#include "lib/Error.h"


// isNaN(value)
js_val *
global_is_nan(js_val *instance, js_args *args, eval_state *state)
{
  js_val *num = TO_NUM(ARG(args, 0));
  return JSBOOL(num->number.is_nan);
}

// isFinite(number)
js_val *
global_is_finite(js_val *instance, js_args *args, eval_state *state)
{
  js_val *num = TO_NUM(ARG(args, 0));
  return JSBOOL(!(num->number.is_nan || num->number.is_inf));
}

// parseInt(string[, radix])
js_val *
global_parse_int(js_val *instance, js_args *args, eval_state *state)
{
  js_val *to_parse = TO_STR(ARG(args, 0));
  js_val *radix_arg = ARG(args, 1);
  
  // FIXME: determine radix based on start of string
  int radix = IS_NUM(radix_arg) ? radix_arg->number.val : 10;
  char *ep;
  long l;

  l = strtol(to_parse->string.ptr, &ep, radix);
  if (*ep != 0) {
    js_val *num = TO_NUM(to_parse);
    return IS_NAN(num) ? JSNAN() : JSNUM(floor(num->number.val));
  }

  return JSNUM((int)l);
}

// parseFloat(string)
js_val *
global_parse_float(js_val *instance, js_args *args, eval_state *state)
{
  return TO_NUM(ARG(args, 0));
}

// eval(string)
js_val *
global_eval(js_val *instance, js_args *args, eval_state *state)
{
  // TODO
  return JSUNDEF();
}

// gc()
js_val *
global_gc(js_val *instance, js_args *args, eval_state *state)
{
  fh_gc();
  return JSUNDEF();
}

// load(filename)
//
// Custom Flathead function. Execute the given file in the 
// global scope. Parsing and evaluation occur at runtime.
js_val *
global_load(js_val *instance, js_args *args, eval_state *state)
{
  js_val *name = TO_STR(ARG(args, 0));

  FILE *file = fopen(name->string.ptr, "r");
  fh_eval_file(file, fh->global, false);

  return JSUNDEF();
}

// Helper function that attaches a given prototype to all properties of the
// given object that are native functions.
void
fh_attach_prototype(js_val *obj, js_val *proto)
{
  js_prop *prop;
  OBJ_ITER(obj, prop) {
    if (prop->ptr && IS_FUNC(prop->ptr) && prop->ptr->object.native)
      prop->ptr->proto = proto;
  }
}

js_val *
fh_bootstrap()
{
  js_val *global = JSOBJ();

  fh_set(global, "Object", bootstrap_object());
  fh_set(global, "Function", bootstrap_function());
  fh_set(global, "Array", bootstrap_array());
  fh_set(global, "String", bootstrap_string());
  fh_set(global, "Number", bootstrap_number());
  fh_set(global, "Boolean", bootstrap_boolean());
  fh_set(global, "Date", bootstrap_date());
  fh_set(global, "RegExp", bootstrap_regexp());
  fh_set(global, "Error", bootstrap_error());

  fh_attach_prototype(fh->object_proto, fh->function_proto);
  fh_attach_prototype(fh->function_proto, fh->function_proto);

  fh_set(global, "console", bootstrap_console());
  fh_set(global, "Math", bootstrap_math());
  fh_set(global, "NaN", JSNAN());
  fh_set(global, "Infinity", JSINF());
  fh_set(global, "isNaN", JSNFUNC(global_is_nan));
  fh_set(global, "isFinite", JSNFUNC(global_is_finite));
  fh_set(global, "parseInt", JSNFUNC(global_parse_int));
  fh_set(global, "parseFloat", JSNFUNC(global_parse_float));
  fh_set(global, "eval", JSNFUNC(global_eval));
  fh_set(global, "load", JSNFUNC(global_load));
  fh_set(global, "undefined", JSUNDEF());
  fh_set(global, "this", global);

#ifdef fh_gc_expose
  fh_set(global, "gc", JSNFUNC(global_gc));
#endif

  return global;
}
