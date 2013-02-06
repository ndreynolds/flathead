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

  fh_set_class(math, "Math");

  // Properties
  /* Generated with GHC - the Glasgow Haskell Compiler
   * echo "import Data.Number.CReal; main = putStrLn $ showCReal 36 pi" |runghc
   * e:          (exp 1)
   * ln(2):      log(2)
   * ln(10):     log(10)
   * log2(e):    (log(exp 1)/log(2))
   * log10(e):   (log(exp 1)/log(10))
   * pi:         pi
   * pi/2:       (pi/2)
   * pi/4:       (pi/4)
   * 1/pi:       (1/pi)
   * 2/pi:       (2/pi)
   * 2/sqrt(pi): (2/sqrt(pi))
   * 1/sqrt(2):  (1/sqrt(2))
   * SQRT2:      (sqrt(2))
   */
  DEF(math, "E",          JSNUM(2.718281828459045235360287471352662498L));
  DEF(math, "LN2",        JSNUM(0.693147180559945309417232121458176568L));
  DEF(math, "LN10",       JSNUM(2.302585092994045684017991454684364208L));
  DEF(math, "LOG2E",      JSNUM(1.442695040888963407359924681001892137L));
  DEF(math, "LOG10E",     JSNUM(0.434294481903251827651128918916605082L));
  DEF(math, "PI",         JSNUM(3.141592653589793238462643383279502884L));
  DEF(math, "M_PI_2",     JSNUM(1.570796326794896619231321691639751442L));
  DEF(math, "M_PI_4",     JSNUM(0.785398163397448309615660845819875721L));
  DEF(math, "M_1_PI",     JSNUM(0.318309886183790671537767526745028724L));
  DEF(math, "M_2_PI",     JSNUM(0.636619772367581343075535053490057448L));
  DEF(math, "M_2_SQRTPI", JSNUM(1.128379167095512573896158903121545172L));
  DEF(math, "SQRT1_2",    JSNUM(0.707106781186547524400844362104849039L));
  DEF(math, "SQRT2",      JSNUM(1.414213562373095048801688724209698079L));

  // Methods
  DEF(math, "abs", JSNFUNC(math_abs, 1));
  DEF(math, "acos", JSNFUNC(math_acos, 1));
  DEF(math, "asin", JSNFUNC(math_asin, 1));
  DEF(math, "atan", JSNFUNC(math_atan, 1));
  DEF(math, "atan2", JSNFUNC(math_atan2, 2));
  DEF(math, "ceil", JSNFUNC(math_ceil, 1));
  DEF(math, "cos", JSNFUNC(math_cos, 1));
  DEF(math, "exp", JSNFUNC(math_exp, 1));
  DEF(math, "floor", JSNFUNC(math_floor, 1));
  DEF(math, "log", JSNFUNC(math_log, 1));
  DEF(math, "max", JSNFUNC(math_max, 2));
  DEF(math, "min", JSNFUNC(math_min, 2));
  DEF(math, "pow", JSNFUNC(math_pow, 2));
  DEF(math, "random", JSNFUNC(math_random, 0));
  DEF(math, "round", JSNFUNC(math_round, 1));
  DEF(math, "sin", JSNFUNC(math_sin, 1));
  DEF(math, "sqrt", JSNFUNC(math_sqrt, 1));
  DEF(math, "tan", JSNFUNC(math_tan, 1));

  fh_attach_prototype(math, fh->function_proto);

  return math;
}
