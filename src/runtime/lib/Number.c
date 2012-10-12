// Number.c
// --------
// TODO: Work on precision 

#include <float.h>
#include <math.h>
#include "Number.h"

// Number.prototype.toExponential([fractionalDigits])
JSValue *
number_proto_to_exponential(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *digits = ARG0(args);

  if (instance->number.is_nan || instance->number.is_inf) 
    return JSCAST(instance, T_STRING);

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
  int digits = ARG0(args)->type == T_NUMBER ? ARG0(args)->number.val : 0;
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
  JSValue *precision = ARG0(args);
  if (precision->type == T_UNDEF)
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
  return JSCAST(instance, T_STRING);
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
  fh_set(number, "prototype", prototype);
  fh_set(number, "MAX_VALUE", JSNUM(DBL_MAX));
  fh_set(number, "MIN_VALUE", JSNUM(DBL_MIN));
  fh_set(number, "NEGATIVE_INFINITY", JSNINF());
  fh_set(number, "POSITIVE_INFINITY", JSINF());
  fh_set(number, "NaN", JSNAN());

  // Number.prototype
  // ----------------
  
  fh_set(prototype, "toExponential", JSNFUNC(&number_proto_to_exponential));
  fh_set(prototype, "toFixed", JSNFUNC(&number_proto_to_fixed));
  fh_set(prototype, "toLocaleString", JSNFUNC(&number_proto_to_locale_string));
  fh_set(prototype, "toPrecision", JSNFUNC(&number_proto_to_precision));
  fh_set(prototype, "toString", JSNFUNC(&number_proto_to_string));
  fh_set(prototype, "valueOf", JSNFUNC(&number_proto_value_of));

  return number;
}
