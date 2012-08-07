// runtime.c
// ---------
// Bootstrap global object and friends

#include "runtime.h"
#include "../lib/console.h"

JLVALUE *
jl_bootstrap()
{
  JLVALUE *global = JLOBJ();

  jl_assign(global, "console", bootstrap_console());
  jl_assign(global, "NaN", JLNAN());
  jl_assign(global, "Infinity", JLINF());
  jl_assign(global, "this", global);

  return global;
}
