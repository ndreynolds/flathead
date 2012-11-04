// Math.c
// ------
// Implementation of the Math object methods -- light wrappers 
// around math.h library functions.

#include <math.h>
#include <time.h>
#include <unistd.h>

#include "Math.h"

// Math.abs(x)
js_val *
math_abs(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  if (x->number.is_nan) return JSNAN();
  if (x->number.is_inf) return JSINF();
  return JSNUM(fabs(x->number.val));
}

// Math.acos(x)
js_val *
math_acos(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  return JSNUM(acos(x->number.val));
}

// Math.asin(x)
js_val *
math_asin(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  return JSNUM(asin(x->number.val));
}

// Math.atan(x)
js_val *
math_atan(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  return JSNUM(atan(x->number.val));
}

// Math.atan2(y, x)
js_val *
math_atan2(js_val *instance, js_args *args, eval_state *state)
{
  double y = TO_NUM(ARG(args, 0))->number.val;
  double x = TO_NUM(ARG(args, 1))->number.val;
  return JSNUM(atan2(y, x));
}

// Math.ceil(x)
js_val *
math_ceil(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  if (x->number.is_inf) 
    return x->number.is_neg ? JSNINF() : JSINF();
  return JSNUM(ceil(x->number.val));
}

// Math.cos(x)
js_val *
math_cos(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  return JSNUM(cos(x->number.val));
}

// Math.exp(x)
js_val *
math_exp(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  return JSNUM(exp(x->number.val));
}

// Math.floor(x)
js_val *
math_floor(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  if (x->number.is_inf) 
    return x->number.is_neg ? JSNINF() : JSINF();
  return JSNUM(floor(x->number.val));
}

// Math.log(x)
js_val *
math_log(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  return JSNUM(log(x->number.val));
}

// Math.max([value1[,value2[, ...]]])
js_val *
math_max(js_val *instance, js_args *args, eval_state *state)
{
  int length = ARGLEN(args);
  if (length == 0) return JSNINF();
  if (length == 2) {
    js_val *x = TO_NUM(ARG(args, 0)),
            *y = TO_NUM(ARG(args, 1));
    if (x->number.is_nan || y->number.is_nan) return JSNAN();
    return x->number.val > y->number.val ? x : y;
  }

  int i;
  js_val *max = TO_NUM(ARG(args, 0));
  js_val *x;
  if (max->number.is_nan) return JSNAN();
  for (i = 0; i < (length - 1); i++) {
    x = TO_NUM(ARG(args, i+1));
    if (x->number.is_nan) return JSNAN();
    if (x->number.is_inf || x->number.val > max->number.val) 
      max = x;
  };
  return max;
}

// Math.min([value1[,value2[, ...]]])
js_val *
math_min(js_val *instance, js_args *args, eval_state *state)
{
  int length = ARGLEN(args);
  if (length == 0) return JSINF();
  if (length == 2) {
    js_val *x = TO_NUM(ARG(args, 0)),
            *y = TO_NUM(ARG(args, 1));
    if (x->number.is_nan || y->number.is_nan) return JSNAN();
    return x->number.val < y->number.val ? x : y;
  }

  int i;
  js_val *min = TO_NUM(ARG(args, 0));
  js_val *x;
  if (min->number.is_nan) return JSNAN();
  for (i = 0; i < (length - 1); i++) {
    x = TO_NUM(ARG(args, i+1));
    if (x->number.is_nan) return JSNAN();
    if (x->number.val < min->number.val) 
      min = x;
  };
  return min;
}

// Math.pow(x, y)
js_val *
math_pow(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  js_val *y = TO_NUM(ARG(args, 1));
  if (x->number.is_nan || y->number.is_nan)
    return JSNAN();
  return JSNUM(pow(x->number.val, y->number.val));
}

// Math.random()
js_val *
math_random(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM((double)rand() / RAND_MAX);
}

// Math.round(x)
js_val *
math_round(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  return JSNUM(floor(x->number.val + 0.5));
}

// Math.sin(x)
js_val *
math_sin(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  return JSNUM(sin(x->number.val));
}

// Math.sqrt(x)
js_val *
math_sqrt(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  if (x->number.is_nan) return JSNAN();
  if (x->number.is_inf) return JSINF();
  if (x->number.val < 0) return JSNAN();
  return JSNUM(sqrt(x->number.val));
}

// Math.tan(x)
js_val *
math_tan(js_val *instance, js_args *args, eval_state *state)
{
  js_val *x = TO_NUM(ARG(args, 0));
  return JSNUM(tan(x->number.val));
}

unsigned long 
mix(unsigned long a, unsigned long b, unsigned long c)
{
  a=a-b;  a=a-c;  a=a^(c >> 13);
  b=b-c;  b=b-a;  b=b^(a << 8);
  c=c-a;  c=c-b;  c=c^(b >> 13);
  a=a-b;  a=a-c;  a=a^(c >> 12);
  b=b-c;  b=b-a;  b=b^(a << 16);
  c=c-a;  c=c-b;  c=c^(b >> 5);
  a=a-b;  a=a-c;  a=a^(c >> 3);
  b=b-c;  b=b-a;  b=b^(a << 10);
  c=c-a;  c=c-b;  c=c^(b >> 15);
  return c;
}

void 
seed_rand()
{
  // Need more entropy than time alone provides. 
  srand(mix(clock(), time(NULL), getpid()));
}

js_val *
bootstrap_math()
{
  seed_rand();

  js_val *math = JSOBJ();

  // Properties
  BUILTIN(math, "E", JSNUM(M_E));
  BUILTIN(math, "LN2", JSNUM(M_LN2));
  BUILTIN(math, "LN10", JSNUM(M_LN10));
  BUILTIN(math, "LOG2E", JSNUM(M_LOG2E));
  BUILTIN(math, "LOG10E", JSNUM(M_LOG10E));
  BUILTIN(math, "PI", JSNUM(M_PI));
  BUILTIN(math, "SQRT1_2", JSNUM(M_SQRT1_2));
  BUILTIN(math, "SQRT2", JSNUM(M_SQRT2));

  // Methods
  BUILTIN(math, "abs", JSNFUNC(math_abs));
  BUILTIN(math, "acos", JSNFUNC(math_acos));
  BUILTIN(math, "asin", JSNFUNC(math_asin));
  BUILTIN(math, "atan", JSNFUNC(math_atan));
  BUILTIN(math, "atan2", JSNFUNC(math_atan2));
  BUILTIN(math, "ceil", JSNFUNC(math_ceil));
  BUILTIN(math, "cos", JSNFUNC(math_cos));
  BUILTIN(math, "exp", JSNFUNC(math_exp));
  BUILTIN(math, "floor", JSNFUNC(math_floor));
  BUILTIN(math, "log", JSNFUNC(math_log));
  BUILTIN(math, "max", JSNFUNC(math_max));
  BUILTIN(math, "min", JSNFUNC(math_min));
  BUILTIN(math, "pow", JSNFUNC(math_pow));
  BUILTIN(math, "random", JSNFUNC(math_random));
  BUILTIN(math, "round", JSNFUNC(math_round));
  BUILTIN(math, "sin", JSNFUNC(math_sin));
  BUILTIN(math, "sqrt", JSNFUNC(math_sqrt));
  BUILTIN(math, "tan", JSNFUNC(math_tan));

  return math;
}
