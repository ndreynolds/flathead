// Number.c
// --------

#include <float.h>
#include "Number.h"

JLVALUE *
bootstrap_number()
{
  JLVALUE *number = JLOBJ();

  jl_set(number, "MAX_VALUE", JLNUM(DBL_MAX));
  jl_set(number, "MIN_VALUE", JLNUM(DBL_MIN));
  jl_set(number, "NEGATIVE_INFINITY", JLINF()); // TODO: +/-
  jl_set(number, "POSITIVE_INFINITY", JLINF());
  jl_set(number, "NaN", JLNAN());

  return number;
}
