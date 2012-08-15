// Math.c
// ------
// Implementation of the Math object methods -- light wrappers 
// around C library functions.

#include <math.h>
#include "Math.h"

JLVALUE *
math_abs(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  if (arg->number.is_nan) return JLNAN();
  return JLNUM(fabs(arg->number.val));
}

JLVALUE *
math_acos(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  return JLNUM(acos(arg->number.val));
}

JLVALUE *
math_asin(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  return JLNUM(asin(arg->number.val));
}

JLVALUE *
math_atan(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  return JLNUM(atan(arg->number.val));
}

JLVALUE *
math_atan2(JLARGS *args)
{
  // TODO
  /*
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  return JLNUM(atan2(arg->number.val));
  */
}

JLVALUE *
math_ceil(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  return JLNUM(ceil(arg->number.val));
}

JLVALUE *
math_cos(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  return JLNUM(cos(arg->number.val));
}

JLVALUE *
math_exp(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  return JLNUM(exp(arg->number.val));
}

JLVALUE *
math_floor(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  return JLNUM(floor(arg->number.val));
}

JLVALUE *
math_log(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  return JLNUM(log(arg->number.val));
}

JLVALUE *
math_max(JLARGS *args)
{
  // TODO
}

JLVALUE *
math_min(JLARGS *args)
{
  // TODO
}

JLVALUE *
math_pow(JLARGS *args)
{
  // TODO
}

JLVALUE *
math_random(JLARGS *args)
{
  // TODO
}

JLVALUE *
math_round(JLARGS *args)
{
  // TODO
}

JLVALUE *
math_sin(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  return JLNUM(sin(arg->number.val));
}

JLVALUE *
math_sqrt(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  if (arg->number.is_nan) return JLNAN();
  if (arg->number.is_inf) return JLINF();
  if (arg->number.val < 0) return JLNAN();
  return JLNUM(sqrt(arg->number.val));
}

JLVALUE *
math_tan(JLARGS *args)
{
  JLVALUE *arg = JLCAST(GETARG(args), T_NUMBER);
  return JLNUM(tan(arg->number.val));
}

JLVALUE *
bootstrap_math()
{
  JLVALUE *math = JLOBJ();

  jl_set(math, "abs", JLNFUNC((JLNATVFUNC)&math_abs));
  jl_set(math, "acos", JLNFUNC((JLNATVFUNC)&math_acos));
  jl_set(math, "asin", JLNFUNC((JLNATVFUNC)&math_asin));
  jl_set(math, "atan", JLNFUNC((JLNATVFUNC)&math_atan));
  jl_set(math, "atan2", JLNFUNC((JLNATVFUNC)&math_atan2));
  jl_set(math, "ceil", JLNFUNC((JLNATVFUNC)&math_ceil));
  jl_set(math, "cos", JLNFUNC((JLNATVFUNC)&math_cos));
  jl_set(math, "exp", JLNFUNC((JLNATVFUNC)&math_exp));
  jl_set(math, "floor", JLNFUNC((JLNATVFUNC)&math_floor));
  jl_set(math, "log", JLNFUNC((JLNATVFUNC)&math_log));
  jl_set(math, "max", JLNFUNC((JLNATVFUNC)&math_max));
  jl_set(math, "min", JLNFUNC((JLNATVFUNC)&math_min));
  jl_set(math, "pow", JLNFUNC((JLNATVFUNC)&math_pow));
  jl_set(math, "random", JLNFUNC((JLNATVFUNC)&math_random));
  jl_set(math, "round", JLNFUNC((JLNATVFUNC)&math_round));
  jl_set(math, "sin", JLNFUNC((JLNATVFUNC)&math_sin));
  jl_set(math, "sqrt", JLNFUNC((JLNATVFUNC)&math_sqrt));
  jl_set(math, "tan", JLNFUNC((JLNATVFUNC)&math_tan));

  return math;
}
