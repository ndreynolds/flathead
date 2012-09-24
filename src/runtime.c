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
#include "gc.h"
#include "../lib/console.h"
#include "../lib/Math.h"
#include "../lib/Object.h"
#include "../lib/Array.h"
#include "../lib/Number.h"

JSValue *
is_nan(JSValue *instance, JSArgs *args, State *state)
{
  if (args->arg != NULL) {
    JSValue *num = JSCAST((JSValue *)args->arg, T_NUMBER);
    return JSBOOL(num->number.is_nan);
  }
  // undefined also coerces to NaN.
  return JSBOOL(1);
}

JSValue *
is_finite(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *num = JSCAST(args->arg == NULL ? JSUNDEF() : args->arg, T_NUMBER);
  return JSBOOL(!(num->number.is_nan || num->number.is_inf));
}

JSValue *
parse_int(JSValue *instance, JSArgs *args, State *state)
{
  // Ecma 15.1.2.2
  // TODO: use radix argument, strip whitespace.
  JSValue *num = JSCAST(args->arg == NULL ? JSUNDEF() : args->arg, T_NUMBER);
  if (num->number.is_nan) return JSNAN();
  return JSNUM(floor(num->number.val));
}

JSValue *
parse_float(JSValue *instance, JSArgs *args, State *state)
{
  return JSCAST(args->arg == NULL ? JSUNDEF() : args->arg, T_NUMBER);
}

JSValue *
fh_bootstrap()
{
  JSValue *global = JSOBJ();

  fh_gc_register_global(global);

  fh_set(global, "Object", bootstrap_object());
  fh_set(global, "Array", bootstrap_array());
  fh_set(global, "Number", bootstrap_number());

  fh_set(global, "console", bootstrap_console());
  fh_set(global, "Math", bootstrap_math());
  fh_set(global, "NaN", JSNAN());
  fh_set(global, "Infinity", JSINF());
  fh_set(global, "isNaN", JSNFUNC(&is_nan));
  fh_set(global, "isFinite", JSNFUNC(&is_finite));
  fh_set(global, "parseInt", JSNFUNC(&parse_int));
  fh_set(global, "parseFloat", JSNFUNC(&parse_float));
  fh_set(global, "undefined", JSUNDEF());
  fh_set(global, "this", global);

  return global;
}
