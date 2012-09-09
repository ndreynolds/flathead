// console.c
// ---------
// Native implementations for the `console` property of the global object.

#include "console.h"

JSValue *
console_log(JSArgs *args, State *state)
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

JSValue *
console_error(JSArgs *args, State *state)
{
  // https://developer.mozilla.org/en/DOM/console.error
  // TODO: Change JSDEBUG to accept an output stream
  console_log(args, state);
  return JSUNDEF();
}

JSValue *
console_assert(JSArgs *args, State *state)
{
  // Non-standard, found in new Webkit builds and Firebug

  if (args->arg != 0) {
    JSValue *result = JSCAST((JSValue *)args->arg, T_BOOLEAN);
    if (result->boolean.val) return JSUNDEF();
  }
  fh_error(state, "AssertionError");
}

JSValue *
console_time(JSArgs *args, State *state)
{
  // https://developer.mozilla.org/en/DOM/console.time
  // TODO
  return JSUNDEF();
}

JSValue *
bootstrap_console()
{
  JSValue *console = JSOBJ();

  fh_set(console, "log", JSNFUNC(&console_log));
  fh_set(console, "error", JSNFUNC(&console_error));
  fh_set(console, "assert", JSNFUNC(&console_assert));
  fh_set(console, "time", JSNFUNC(&console_time));

  return console;
}
