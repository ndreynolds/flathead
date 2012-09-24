// Math.c
// ------
// Implementation of the Math object methods -- light wrappers 
// around math.h library functions.

#include <math.h>
#include <time.h>
#include <unistd.h>
#include "Math.h"

// Math.abs(x)
JSValue *
math_abs(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  if (x->number.is_nan) return JSNAN();
  if (x->number.is_inf) return JSINF();
  return JSNUM(fabs(x->number.val));
}

// Math.acos(x)
JSValue *
math_acos(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  return JSNUM(acos(x->number.val));
}

// Math.asin(x)
JSValue *
math_asin(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  return JSNUM(asin(x->number.val));
}

// Math.atan(x)
JSValue *
math_atan(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  return JSNUM(atan(x->number.val));
}

// Math.atan2(y, x)
JSValue *
math_atan2(JSValue *instance, JSArgs *args, State *state)
{
  double y = JSCAST(ARG0(args), T_NUMBER)->number.val;
  double x = JSCAST(ARGN(args, 1), T_NUMBER)->number.val;
  return JSNUM(atan2(y, x));
}

// Math.ceil(x)
JSValue *
math_ceil(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  if (x->number.is_inf) 
    return x->number.is_neg ? JSNINF() : JSINF();
  return JSNUM(ceil(x->number.val));
}

// Math.cos(x)
JSValue *
math_cos(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  return JSNUM(cos(x->number.val));
}

// Math.exp(x)
JSValue *
math_exp(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  return JSNUM(exp(x->number.val));
}

// Math.floor(x)
JSValue *
math_floor(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  if (x->number.is_inf) 
    return x->number.is_neg ? JSNINF() : JSINF();
  return JSNUM(floor(x->number.val));
}

// Math.log(x)
JSValue *
math_log(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  return JSNUM(log(x->number.val));
}

// Math.max([value1[,value2[, ...]]])
JSValue *
math_max(JSValue *instance, JSArgs *args, State *state)
{
  int length = ARGLEN(args);
  if (length == 0) return JSNINF();
  if (length == 2) {
    JSValue *x = JSCAST(ARGN(args, 0), T_NUMBER),
            *y = JSCAST(ARGN(args, 1), T_NUMBER);
    if (x->number.is_nan || y->number.is_nan) return JSNAN();
    return x->number.val > y->number.val ? x : y;
  }

  int i;
  JSValue *max = JSCAST(ARGN(args, 0), T_NUMBER);
  JSValue *x;
  if (max->number.is_nan) return JSNAN();
  for (i=0; i<(length-1); i++) {
    x = JSCAST(ARGN(args, i+1), T_NUMBER);
    if (x->number.is_nan) return JSNAN();
    if (x->number.is_inf || x->number.val > max->number.val) 
      max = x;
  };
  return max;
}

// Math.min([value1[,value2[, ...]]])
JSValue *
math_min(JSValue *instance, JSArgs *args, State *state)
{
  int length = ARGLEN(args);
  if (length == 0) return JSINF();
  if (length == 2) {
    JSValue *x = JSCAST(ARGN(args, 0), T_NUMBER),
            *y = JSCAST(ARGN(args, 1), T_NUMBER);
    if (x->number.is_nan || y->number.is_nan) return JSNAN();
    return x->number.val < y->number.val ? x : y;
  }

  int i;
  JSValue *min = JSCAST(ARGN(args, 0), T_NUMBER);
  JSValue *x;
  if (min->number.is_nan) return JSNAN();
  for (i=0; i<(length-1); i++) {
    x = JSCAST(ARGN(args, i+1), T_NUMBER);
    if (x->number.is_nan) return JSNAN();
    if (x->number.val < min->number.val) 
      min = x;
  };
  return min;
}

// Math.pow(x, y)
JSValue *
math_pow(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARGN(args, 0), T_NUMBER);
  JSValue *y = JSCAST(ARGN(args, 1), T_NUMBER);
  if (x->number.is_nan || y->number.is_nan)
    return JSNAN();
  return JSNUM(pow(x->number.val, y->number.val));
}

// Math.random()
JSValue *
math_random(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM((double)rand() / RAND_MAX);
}

// Math.round(x)
JSValue *
math_round(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  return JSNUM(floor(x->number.val + 0.5));
}

// Math.sin(x)
JSValue *
math_sin(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  return JSNUM(sin(x->number.val));
}

// Math.sqrt(x)
JSValue *
math_sqrt(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
  if (x->number.is_nan) return JSNAN();
  if (x->number.is_inf) return JSINF();
  if (x->number.val < 0) return JSNAN();
  return JSNUM(sqrt(x->number.val));
}

// Math.tan(x)
JSValue *
math_tan(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *x = JSCAST(ARG0(args), T_NUMBER);
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

JSValue *
bootstrap_math()
{
  seed_rand();

  JSValue *math = JSOBJ();

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
  BUILTIN(math, "abs", JSNFUNC(&math_abs));
  BUILTIN(math, "acos", JSNFUNC(&math_acos));
  BUILTIN(math, "asin", JSNFUNC(&math_asin));
  BUILTIN(math, "atan", JSNFUNC(&math_atan));
  BUILTIN(math, "atan2", JSNFUNC(&math_atan2));
  BUILTIN(math, "ceil", JSNFUNC(&math_ceil));
  BUILTIN(math, "cos", JSNFUNC(&math_cos));
  BUILTIN(math, "exp", JSNFUNC(&math_exp));
  BUILTIN(math, "floor", JSNFUNC(&math_floor));
  BUILTIN(math, "log", JSNFUNC(&math_log));
  BUILTIN(math, "max", JSNFUNC(&math_max));
  BUILTIN(math, "min", JSNFUNC(&math_min));
  BUILTIN(math, "pow", JSNFUNC(&math_pow));
  BUILTIN(math, "random", JSNFUNC(&math_random));
  BUILTIN(math, "round", JSNFUNC(&math_round));
  BUILTIN(math, "sin", JSNFUNC(&math_sin));
  BUILTIN(math, "sqrt", JSNFUNC(&math_sqrt));
  BUILTIN(math, "tan", JSNFUNC(&math_tan));

  return math;
}
