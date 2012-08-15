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
#include "../lib/console.h"
#include "../lib/Math.h"
#include "../lib/Number.h"

JLVALUE *
jl_is_nan(JLARGS *args)
{
  if (args->arg != NULL) {
    JLVALUE *num = JLCAST((JLVALUE *)args->arg, T_NUMBER);
    if (num->number.is_nan) return JLBOOL(1);
    return JLBOOL(0);
  }
  // undefined also coerces to NaN.
  return JLBOOL(1);
}

JLVALUE *
jl_is_finite(JLARGS *args)
{
  JLVALUE *num = JLCAST(args->arg == NULL ? JLUNDEF() : args->arg, T_NUMBER);
  if (num->number.is_nan || num->number.is_inf) return JLBOOL(0);
  return JLBOOL(1);
}

JLVALUE *
jl_parse_int(JLARGS *args)
{
  // Ecma 15.1.2.2
  // TODO: use radix argument, strip whitespace.
  JLVALUE *num = JLCAST(args->arg == NULL ? JLUNDEF() : args->arg, T_NUMBER);
  if (num->number.is_nan) return JLNAN();
  return JLNUM(floor(num->number.val));
}

JLVALUE *
jl_parse_float(JLARGS *args)
{
  return JLCAST(args->arg == NULL ? JLUNDEF() : args->arg, T_NUMBER);
}

JLVALUE *
jl_bootstrap()
{
  JLVALUE *global = JLOBJ();

  jl_set(global, "console", bootstrap_console());
  jl_set(global, "Math", bootstrap_math());
  jl_set(global, "Number", bootstrap_number());
  jl_set(global, "NaN", JLNAN());
  jl_set(global, "Infinity", JLINF());
  jl_set(global, "isNaN", JLNFUNC((JLNATVFUNC)&jl_is_nan));
  jl_set(global, "isFinite", JLNFUNC((JLNATVFUNC)&jl_is_finite));
  jl_set(global, "parseInt", JLNFUNC((JLNATVFUNC)&jl_parse_int));
  jl_set(global, "parseFloat", JLNFUNC((JLNATVFUNC)&jl_parse_float));
  jl_set(global, "undefined", JLUNDEF());
  jl_set(global, "this", global);

  return global;
}
