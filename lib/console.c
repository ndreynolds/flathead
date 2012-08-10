// console.c
// ---------
// Native implementations for the `console` property of the global object.

#include "console.h"

void
console_log(JLARGS *args)
{
  // https://developer.mozilla.org/en/DOM/console.log
  bool first = true;
  while(first || args->next != 0)
  {
    if (!first)
      args = args->next;
    if (args->arg != 0)
      JLDEBUG(args->arg);
    first = false;
  }
}

void
console_error(JLARGS *args)
{
  // https://developer.mozilla.org/en/DOM/console.error
  // TODO: Change JLDEBUG to accept an output stream
  console_log(args);
}

void
console_time(JLARGS *args)
{
  // https://developer.mozilla.org/en/DOM/console.time
}

void
console_assert(JLARGS *args)
{
  // Non-standard, found in new Webkit builds and Firebug

  if (args->arg != 0) {
    JLVALUE *result = JLCAST((JLVALUE *)args->arg, T_BOOLEAN);
    if (result->boolean.val) return;
  }
  fprintf(stderr, "Assertion failed!");
  exit(1);
}

JLVALUE *
bootstrap_console()
{
  JLVALUE *console = JLOBJ();

  jl_assign(console, "log", JLNFUNC((JLNATVFUNC)&console_log));
  jl_assign(console, "error", JLNFUNC((JLNATVFUNC)&console_error));
  jl_assign(console, "time", JLNFUNC((JLNATVFUNC)&console_time));
  jl_assign(console, "assert", JLNFUNC((JLNATVFUNC)&console_assert));

  return console;
}
