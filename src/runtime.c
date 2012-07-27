// runtime.c
// ---------
// Bootstrap global object and friends

#include "runtime.h"
#include "../lib/console.h"

JLVALUE *
jl_bootstrap()
{
  JLVALUE *global = JLOBJ();

  jl_prop_assign_obj(global, "console", bootstrap_console());
  jl_prop_assign_obj(global, "NaN", JLNAN());
  jl_prop_assign_obj(global, "Infinity", JLINF());

  return global;
}
