// console.c
// ---------
// Native implementations for the `console` property of the global object.

#include "console.h"

JSVALUE *
console_log(JSARGS *args)
{
  // https://developer.mozilla.org/en/DOM/console.log
  bool first = true;
  while(first || args->next != 0)
  {
    if (!first)
      args = args->next;
    if (args->arg != 0)
      JSDEBUG(args->arg);
    first = false;
  }
  return JSUNDEF();
}

JSVALUE *
console_error(JSARGS *args)
{
  // https://developer.mozilla.org/en/DOM/console.error
  // TODO: Change JSDEBUG to accept an output stream
  console_log(args);
  return JSUNDEF();
}

JSVALUE *
console_time(JSARGS *args)
{
  // https://developer.mozilla.org/en/DOM/console.time
  // TODO
  return JSUNDEF();
}

JSVALUE *
console_assert(JSARGS *args)
{
  // Non-standard, found in new Webkit builds and Firebug

  if (args->arg != 0) {
    JSVALUE *result = JSCAST((JSVALUE *)args->arg, T_BOOLEAN);
    if (result->boolean.val) return JSUNDEF();
  }
  fprintf(stderr, "Assertion failed!");
  exit(1);
}

JSVALUE *
bootstrap_console()
{
  JSVALUE *console = JSOBJ();

  fh_set(console, "log", JSNFUNC((JSNATVFUNC)&console_log));
  fh_set(console, "error", JSNFUNC((JSNATVFUNC)&console_error));
  fh_set(console, "time", JSNFUNC((JSNATVFUNC)&console_time));
  fh_set(console, "assert", JSNFUNC((JSNATVFUNC)&console_assert));

  return console;
}
