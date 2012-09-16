// console.c
// ---------
// Native implementations for the `console` property of the global object.

#include "console.h"

JSValue *
console_log(JSArgs *args, State *state)
{
  // https://developer.mozilla.org/en/DOM/console.log
  log_args(stdout, args);
  return JSUNDEF();
}

JSValue *
console_error(JSArgs *args, State *state)
{
  // https://developer.mozilla.org/en/DOM/console.error
  log_args(stderr, args);
  return JSUNDEF();
}

JSValue *
console_assert(JSArgs *args, State *state)
{
  // Non-standard, found in new Webkit builds and Firebug
  if (args->arg != NULL) {
    JSValue *result = JSCAST((JSValue *)args->arg, T_BOOLEAN);
    if (result->boolean.val) return JSUNDEF();
  }
  fh_error(state, E_ASSERTION, "assertion failed");
  assert(0);
}

JSValue *
console_time(JSArgs *args, State *state)
{
  // https://developer.mozilla.org/en/DOM/console.time
  // TODO
  return JSUNDEF();
}

void
log_args(FILE *stream, JSArgs *args)
{
  bool first = true;
  while(first || args->next != 0)
  {
    if (!first)
      args = args->next;
    if (args->arg != 0)
      fh_debug(stream, args->arg, 0, 1);
    first = false;
  }
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
