// console.c
// ---------
// Native implementations for the `console` property of the global object.

#include "console.h"

void
console_log(JLARGS *args)
{
  fprintf(stdout, "some stuff");
  // https://developer.mozilla.org/en/DOM/console.log
}

void
console_error(JLARGS *args)
{
  fprintf(stderr, "some bad stuff");
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

  jl_assign(console, "log", JLNFUNC((JLNATVFUNC*)&console_log));
  jl_assign(console, "error", JLNFUNC((JLNATVFUNC*)&console_error));
  jl_assign(console, "time", JLNFUNC((JLNATVFUNC*)&console_time));

  return console;
}
