// Number.c
// --------

#include <float.h>
#include "Number.h"

// Number.prototype.toExponential([fractionalDigits])
JSValue *
number_proto_to_exponential(JSValue *instance, JSArgs *args, State *state)
{
  int size = snprintf(NULL, 0, "%e", instance->number.val);
  char *exp_str = malloc(size + 1);
  sprintf(exp_str, "%e", instance->number.val);
  return JSSTR(exp_str);
}

// Number.prototype.toFixed([digits])
JSValue *
number_proto_to_fixed(JSValue *instance, JSArgs *args, State *state)
{
  // TODO: stub
  return JSUNDEF();
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
  // TODO: stub
  return JSUNDEF();
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
