// console.c
// ---------
// Native implementations for the `console` property of the global object.

#include "console.h"

JSValue *
console_log(JSValue *instance, JSArgs *args, State *state)
{
  int i;
  for (i = 0; i < ARGLEN(args); i++)
    fh_debug(stdout, ARG(args, i), 0, 1);
  return JSUNDEF();
}

JSValue *
console_error(JSValue *instance, JSArgs *args, State *state)
{
  int i;
  for (i = 0; i < ARGLEN(args); i++)
    fh_debug(stderr, ARG(args, i), 0, 1);
  return JSUNDEF();
}

JSValue *
console_assert(JSValue *instance, JSArgs *args, State *state)
{
  // Non-standard, found in new Webkit builds and Firebug
  if (TO_BOOL(ARG(args, 0))->boolean.val)
    return JSUNDEF();
  fh_error(state, E_ASSERTION, "assertion failed");
  assert(0);
}

JSValue *
console_time(JSValue *instance, JSArgs *args, State *state)
{
  // https://developer.mozilla.org/en/DOM/console.time
  // TODO
  return JSUNDEF();
}

JSValue *
bootstrap_console()
{
  JSValue *console = JSOBJ();

  BUILTIN(console, "log", JSNFUNC(&console_log));
  BUILTIN(console, "error", JSNFUNC(&console_error));
  BUILTIN(console, "assert", JSNFUNC(&console_assert));
  BUILTIN(console, "time", JSNFUNC(&console_time));

  return console;
}
