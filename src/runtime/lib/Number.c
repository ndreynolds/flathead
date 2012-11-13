// Number.c
// --------
// TODO: Work on precision 

#include <float.h>
#include <math.h>

#include "Number.h"


// new Number(value)
js_val *
number_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *value = ARG(args, 0);
  if (state->construct)
    state->this->object.primitive = TO_NUM(value);
  return TO_NUM(value);
}

// Number.prototype.toExponential([fractionalDigits])
js_val *
number_proto_to_exponential(js_val *instance, js_args *args, eval_state *state)
{
  js_val *digits = ARG(args, 0);

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
js_val *
number_proto_to_fixed(js_val *instance, js_args *args, eval_state *state)
{
  int digits = ARG(args, 0)->type == T_NUMBER ? ARG(args, 0)->number.val : 0;
  int size = snprintf(NULL, 0, "%.*f", digits, instance->number.val);
  char *exp_str = malloc(size + 1);
  sprintf(exp_str, "%.*f", digits, instance->number.val);
  return JSSTR(exp_str);
}

// Number.prototype.toLocaleString()
js_val *
number_proto_to_locale_string(js_val *instance, js_args *args, eval_state *state)
{
  return number_proto_to_string(instance, args, state);
}

// Number.prototype.toPrecision([precision])
js_val *
number_proto_to_precision(js_val *instance, js_args *args, eval_state *state)
{
  js_val *precision = ARG(args, 0);
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
js_val *
number_proto_to_string(js_val *instance, js_args *args, eval_state *state)
{
  return TO_STR(instance);
}

// Number.prototype.valueOf()
js_val *
number_proto_value_of(js_val *instance, js_args *args, eval_state *state)
{
  return instance;
}

js_val *
bootstrap_number()
{
  js_val *number = JSNFUNC(number_new, 1);
  js_val *prototype = JSOBJ();

  // Number
  // ------

  // Properties
  DEF(number, "prototype", prototype);
  DEF(number, "MAX_VALUE", JSNUM(DBL_MAX));
  DEF(number, "MIN_VALUE", JSNUM(DBL_MIN));
  DEF(number, "NEGATIVE_INFINITY", JSNINF());
  DEF(number, "POSITIVE_INFINITY", JSINF());
  DEF(number, "NaN", JSNAN());

  // Number.prototype
  // ----------------
  
  // Properties
  DEF(prototype, "constructor", JSNFUNC(number_new, 1));

  // Methods
  DEF(prototype, "toExponential", JSNFUNC(number_proto_to_exponential, 1));
  DEF(prototype, "toFixed", JSNFUNC(number_proto_to_fixed, 1));
  DEF(prototype, "toLocaleString", JSNFUNC(number_proto_to_locale_string, 0));
  DEF(prototype, "toPrecision", JSNFUNC(number_proto_to_precision, 1));
  DEF(prototype, "toString", JSNFUNC(number_proto_to_string, 1));
  DEF(prototype, "valueOf", JSNFUNC(number_proto_value_of, 0));

  fh_attach_prototype(prototype, fh->function_proto);

  return number;
}
