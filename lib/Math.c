// Math.c
// ------
// Implementation of the Math object methods -- light wrappers 
// around math.h library functions.

#include <math.h>
#include <time.h>
#include <unistd.h>
#include "Math.h"

// Math.abs(x)
JSVALUE *
math_abs(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  if (x->number.is_nan) return JSNAN();
  if (x->number.is_inf) return JSINF();
  return JSNUM(fabs(x->number.val));
}

// Math.acos(x)
JSVALUE *
math_acos(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(acos(x->number.val));
}

// Math.asin(x)
JSVALUE *
math_asin(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(asin(x->number.val));
}

// Math.atan(x)
JSVALUE *
math_atan(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(atan(x->number.val));
}

// Math.atan2(y, x)
JSVALUE *
math_atan2(JSARGS *args)
{
  double y = JSCAST(GETARG(args), T_NUMBER)->number.val;
  double x = JSCAST(GETARGN(args, 1), T_NUMBER)->number.val;
  return JSNUM(atan2(y, x));
}

// Math.ceil(x)
JSVALUE *
math_ceil(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  if (x->number.is_inf) 
    return x->number.is_neg ? JSNINF() : JSINF();
  return JSNUM(ceil(x->number.val));
}

// Math.cos(x)
JSVALUE *
math_cos(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(cos(x->number.val));
}

// Math.exp(x)
JSVALUE *
math_exp(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(exp(x->number.val));
}

// Math.floor(x)
JSVALUE *
math_floor(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  if (x->number.is_inf) 
    return x->number.is_neg ? JSNINF() : JSINF();
  return JSNUM(floor(x->number.val));
}

// Math.log(x)
JSVALUE *
math_log(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(log(x->number.val));
}

// Math.max([value1[,value2[, ...]]])
JSVALUE *
math_max(JSARGS *args)
{
  int length = ARGLEN(args);
  if (length == 0) return JSNINF();
  if (length == 2) {
    JSVALUE *x = JSCAST(GETARGN(args, 0), T_NUMBER),
            *y = JSCAST(GETARGN(args, 1), T_NUMBER);
    if (x->number.is_nan || y->number.is_nan) return JSNAN();
    return x->number.val > y->number.val ? x : y;
  }

  int i;
  JSVALUE *max = JSCAST(GETARGN(args, 0), T_NUMBER);
  JSVALUE *x;
  if (max->number.is_nan) return JSNAN();
  for (i=0; i<(length-1); i++) {
    x = JSCAST(GETARGN(args, i+1), T_NUMBER);
    if (x->number.is_nan) return JSNAN();
    if (x->number.is_inf || x->number.val > max->number.val) 
      max = x;
  };
  return max;
}

// Math.min([value1[,value2[, ...]]])
JSVALUE *
math_min(JSARGS *args)
{
  int length = ARGLEN(args);
  if (length == 0) return JSINF();
  if (length == 2) {
    JSVALUE *x = JSCAST(GETARGN(args, 0), T_NUMBER),
            *y = JSCAST(GETARGN(args, 1), T_NUMBER);
    if (x->number.is_nan || y->number.is_nan) return JSNAN();
    return x->number.val < y->number.val ? x : y;
  }

  int i;
  JSVALUE *min = JSCAST(GETARGN(args, 0), T_NUMBER);
  JSVALUE *x;
  if (min->number.is_nan) return JSNAN();
  for (i=0; i<(length-1); i++) {
    x = JSCAST(GETARGN(args, i+1), T_NUMBER);
    if (x->number.is_nan) return JSNAN();
    if (x->number.val < min->number.val) 
      min = x;
  };
  return min;
}

// Math.pow(x, y)
JSVALUE *
math_pow(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARGN(args, 0), T_NUMBER);
  JSVALUE *y = JSCAST(GETARGN(args, 1), T_NUMBER);
  if (x->number.is_nan || y->number.is_nan)
    return JSNAN();
  return JSNUM(pow(x->number.val, y->number.val));
}

// Math.random()
JSVALUE *
math_random(JSARGS *args)
{
  return JSNUM((double)rand() / RAND_MAX);
}

// Math.round(x)
JSVALUE *
math_round(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(floor(x->number.val + 0.5));
}

// Math.sin(x)
JSVALUE *
math_sin(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(sin(x->number.val));
}

// Math.sqrt(x)
JSVALUE *
math_sqrt(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
  if (x->number.is_nan) return JSNAN();
  if (x->number.is_inf) return JSINF();
  if (x->number.val < 0) return JSNAN();
  return JSNUM(sqrt(x->number.val));
}

// Math.tan(x)
JSVALUE *
math_tan(JSARGS *args)
{
  JSVALUE *x = JSCAST(GETARG(args), T_NUMBER);
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

JSVALUE *
bootstrap_math()
{
  seed_rand();

  JSVALUE *math = JSOBJ();

  // Properties
  fh_set(math, "E", JSNUM(M_E));
  fh_set(math, "LN2", JSNUM(M_LN2));
  fh_set(math, "LN10", JSNUM(M_LN10));
  fh_set(math, "LOG2E", JSNUM(M_LOG2E));
  fh_set(math, "LOG10E", JSNUM(M_LOG10E));
  fh_set(math, "PI", JSNUM(M_PI));
  fh_set(math, "SQRT1_2", JSNUM(M_SQRT1_2));
  fh_set(math, "SQRT2", JSNUM(M_SQRT2));

  // Methods
  fh_set(math, "abs", JSNFUNC((JSNATVFUNC)&math_abs));
  fh_set(math, "acos", JSNFUNC((JSNATVFUNC)&math_acos));
  fh_set(math, "asin", JSNFUNC((JSNATVFUNC)&math_asin));
  fh_set(math, "atan", JSNFUNC((JSNATVFUNC)&math_atan));
  fh_set(math, "atan2", JSNFUNC((JSNATVFUNC)&math_atan2));
  fh_set(math, "ceil", JSNFUNC((JSNATVFUNC)&math_ceil));
  fh_set(math, "cos", JSNFUNC((JSNATVFUNC)&math_cos));
  fh_set(math, "exp", JSNFUNC((JSNATVFUNC)&math_exp));
  fh_set(math, "floor", JSNFUNC((JSNATVFUNC)&math_floor));
  fh_set(math, "log", JSNFUNC((JSNATVFUNC)&math_log));
  fh_set(math, "max", JSNFUNC((JSNATVFUNC)&math_max));
  fh_set(math, "min", JSNFUNC((JSNATVFUNC)&math_min));
  fh_set(math, "pow", JSNFUNC((JSNATVFUNC)&math_pow));
  fh_set(math, "random", JSNFUNC((JSNATVFUNC)&math_random));
  fh_set(math, "round", JSNFUNC((JSNATVFUNC)&math_round));
  fh_set(math, "sin", JSNFUNC((JSNATVFUNC)&math_sin));
  fh_set(math, "sqrt", JSNFUNC((JSNATVFUNC)&math_sqrt));
  fh_set(math, "tan", JSNFUNC((JSNATVFUNC)&math_tan));

  return math;
}
