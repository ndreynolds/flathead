// Number.c
// --------
// TODO: Work on precision 

#include <float.h>
#include <math.h>

#include "Number.h"


// new Number(value)
JSValue *
number_new(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSNUM(0);
}

// Number.prototype.toExponential([fractionalDigits])
JSValue *
number_proto_to_exponential(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *digits = ARG(args, 0);

  if (instance->number.is_nan || instance->number.is_inf) 
    return TO_STR(instance);

  if (digits->type != T_UNDEF) {
    if (digits->number.val < 0 || digits->number.val > 20)
      fh_error(state, E_RANGE, "fractionDigits must be between 0 and 20");
  }

  double x, m;
  int e;
  char *sign;

  x = instance->number.val;
  e = (int)log10(x);
  m = x / pow(10, e);
  if (m < 1) m *= 10, e--;
  sign = e > 0 ? "+" : "";

  char *exp_str;
  int size, ndigits;
  if (digits->type != T_UNDEF) {
    ndigits = digits->number.val;
    size = snprintf(NULL, 0, "%.*fe%s%d", ndigits, m, sign, e);
    exp_str = malloc(size + 1);
    sprintf(exp_str, "%.*fe%s%d", ndigits, m, sign, e);
  }
  else {
    size = snprintf(NULL, 0, "%ge%s%d", m, sign, e);
    exp_str = malloc(size + 1);
    sprintf(exp_str, "%ge%s%d", m, sign, e);
  }
  return JSSTR(exp_str);
}

// Number.prototype.toFixed([digits])
JSValue *
number_proto_to_fixed(JSValue *instance, JSArgs *args, State *state)
{
  int digits = ARG(args, 0)->type == T_NUMBER ? ARG(args, 0)->number.val : 0;
  int size = snprintf(NULL, 0, "%.*f", digits, instance->number.val);
  char *exp_str = malloc(size + 1);
  sprintf(exp_str, "%.*f", digits, instance->number.val);
  return JSSTR(exp_str);
}

// Number.prototype.toLocaleString()
JSValue *
number_proto_to_locale_string(JSValue *instance, JSArgs *args, State *state)
{
  return number_proto_to_string(instance, args, state);
}

// Number.prototype.toPrecision([precision])
JSValue *
number_proto_to_precision(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *precision = ARG(args, 0);
  if (IS_UNDEF(precision))
    return number_proto_to_string(instance, args, state);

  int digits = floor(precision->number.val + 0.5);
  if (digits < 1 || digits > 100)
    fh_error(state, E_RANGE, "precision must be between 1 and 100");

  int size = snprintf(NULL, 0, "%.*g", digits, instance->number.val);
  char *str = malloc(size + 1);
  sprintf(str, "%.*g", digits, instance->number.val);
  return JSSTR(str);
}

// Number.prototype.toString()
JSValue *
number_proto_to_string(JSValue *instance, JSArgs *args, State *state)
{
  return TO_STR(instance);
}

// Number.prototype.valueOf()
JSValue *
number_proto_value_of(JSValue *instance, JSArgs *args, State *state)
{
  return instance;
}

JSValue *
bootstrap_number()
{
  JSValue *number = JSOBJ();
  JSValue *prototype = JSOBJ();

  // Number
  // ------

  // Properties
  BUILTIN(number, "prototype", prototype);
  BUILTIN(number, "MAX_VALUE", JSNUM(DBL_MAX));
  BUILTIN(number, "MIN_VALUE", JSNUM(DBL_MIN));
  BUILTIN(number, "NEGATIVE_INFINITY", JSNINF());
  BUILTIN(number, "POSITIVE_INFINITY", JSINF());
  BUILTIN(number, "NaN", JSNAN());

  // Number.prototype
  // ----------------
  
  // Properties
  BUILTIN(prototype, "constructor", JSNFUNC(&number_new));

  // Methods
  BUILTIN(prototype, "toExponential", JSNFUNC(&number_proto_to_exponential));
  BUILTIN(prototype, "toFixed", JSNFUNC(&number_proto_to_fixed));
  BUILTIN(prototype, "toLocaleString", JSNFUNC(&number_proto_to_locale_string));
  BUILTIN(prototype, "toPrecision", JSNFUNC(&number_proto_to_precision));
  BUILTIN(prototype, "toString", JSNFUNC(&number_proto_to_string));
  BUILTIN(prototype, "valueOf", JSNFUNC(&number_proto_value_of));

  return number;
}
