// Number.c
// --------

#include <float.h>
#include "Number.h"

JSValue *
bootstrap_number()
{
  JSValue *number = JSOBJ();

  fh_set(number, "MAX_VALUE", JSNUM(DBL_MAX));
  fh_set(number, "MIN_VALUE", JSNUM(DBL_MIN));
  fh_set(number, "NEGATIVE_INFINITY", JSNINF());
  fh_set(number, "POSITIVE_INFINITY", JSINF());
  fh_set(number, "NaN", JSNAN());

  return number;
}
