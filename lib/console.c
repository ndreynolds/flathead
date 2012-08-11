// console.c
// ---------
// Native implementations for the `console` property of the global object.

#include "console.h"

JLVALUE *
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
  return JLUNDEF();
}

JLVALUE *
console_error(JLARGS *args)
{
  // https://developer.mozilla.org/en/DOM/console.error
  // TODO: Change JLDEBUG to accept an output stream
  console_log(args);
  return JLUNDEF();
}

JLVALUE *
console_time(JLARGS *args)
{
  // https://developer.mozilla.org/en/DOM/console.time
  // TODO
  return JLUNDEF();
}

JLVALUE *
console_assert(JLARGS *args)
{
  // Non-standard, found in new Webkit builds and Firebug

  if (args->arg != 0) {
    JLVALUE *result = JLCAST((JLVALUE *)args->arg, T_BOOLEAN);
    if (result->boolean.val) return JLUNDEF();
  }
  fprintf(stderr, "Assertion failed!");
  exit(1);
}

JLVALUE *
bootstrap_console()
{
  JLVALUE *console = JLOBJ();

  jl_set(console, "log", JLNFUNC((JLNATVFUNC)&console_log));
  jl_set(console, "error", JLNFUNC((JLNATVFUNC)&console_error));
  jl_set(console, "time", JLNFUNC((JLNATVFUNC)&console_time));
  jl_set(console, "assert", JLNFUNC((JLNATVFUNC)&console_assert));

  return console;
}
