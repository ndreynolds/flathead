// Math.c
// ------
// Implementation of the Math object methods -- light wrappers 
// around math.h library functions.

#include <math.h>
#include "Math.h"

JSVALUE *
math_abs(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  if (arg->number.is_nan) return JSNAN();
  return JSNUM(fabs(arg->number.val));
}

JSVALUE *
math_acos(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(acos(arg->number.val));
}

JSVALUE *
math_asin(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(asin(arg->number.val));
}

JSVALUE *
math_atan(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(atan(arg->number.val));
}

JSVALUE *
math_atan2(JSARGS *args)
{
  // TODO
  /*
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(atan2(arg->number.val));
  */
}

JSVALUE *
math_ceil(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(ceil(arg->number.val));
}

JSVALUE *
math_cos(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(cos(arg->number.val));
}

JSVALUE *
math_exp(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(exp(arg->number.val));
}

JSVALUE *
math_floor(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(floor(arg->number.val));
}

JSVALUE *
math_log(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(log(arg->number.val));
}

JSVALUE *
math_max(JSARGS *args)
{
  // TODO
}

JSVALUE *
math_min(JSARGS *args)
{
  // TODO
}

JSVALUE *
math_pow(JSARGS *args)
{
  // TODO
}

JSVALUE *
math_random(JSARGS *args)
{
  // TODO
}

JSVALUE *
math_round(JSARGS *args)
{
  // TODO
}

JSVALUE *
math_sin(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(sin(arg->number.val));
}

JSVALUE *
math_sqrt(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  if (arg->number.is_nan) return JSNAN();
  if (arg->number.is_inf) return JSINF();
  if (arg->number.val < 0) return JSNAN();
  return JSNUM(sqrt(arg->number.val));
}

JSVALUE *
math_tan(JSARGS *args)
{
  JSVALUE *arg = JSCAST(GETARG(args), T_NUMBER);
  return JSNUM(tan(arg->number.val));
}

JSVALUE *
bootstrap_math()
{
  JSVALUE *math = JSOBJ();

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
