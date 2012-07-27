// console.c
// ---------
// Native implementations for the `console` property of the global object.

#include "console.h"

void
console_log(JLARGS *args)
{
  // https://developer.mozilla.org/en/DOM/console.log
}

void
console_error(JLARGS *args)
{
  // https://developer.mozilla.org/en/DOM/console.error
}

void
console_time(JLARGS *args)
{
  // https://developer.mozilla.org/en/DOM/console.time
}

JLVALUE *
bootstrap_console()
{
  JLVALUE *console = JLOBJ();

  jl_prop_assign_natv_func(console, "log", (JLNATVFUNC*)&console_log);
  jl_prop_assign_natv_func(console, "error", (JLNATVFUNC*)&console_error);
  jl_prop_assign_natv_func(console, "time", (JLNATVFUNC*)&console_time);

  return console;
}
