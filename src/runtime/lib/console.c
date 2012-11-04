// console.c
// ---------
// Native implementations for the `console` property of the global object.

#include "console.h"

js_val *
console_log(js_val *instance, js_args *args, eval_state *state)
{
  int i;
  for (i = 0; i < ARGLEN(args); i++)
    fh_debug(stdout, ARG(args, i), 0, 1);
  return JSUNDEF();
}

js_val *
console_error(js_val *instance, js_args *args, eval_state *state)
{
  int i;
  for (i = 0; i < ARGLEN(args); i++)
    fh_debug(stderr, ARG(args, i), 0, 1);
  return JSUNDEF();
}

js_val *
console_info(js_val *instance, js_args *args, eval_state *state)
{
  int i;
  for (i = 0; i < ARGLEN(args); i++)
    fh_debug_verbose(stderr, ARG(args, i), 0);
  return JSUNDEF();
}

js_val *
console_assert(js_val *instance, js_args *args, eval_state *state)
{
  // Non-standard, found in new Webkit builds and Firebug
  if (TO_BOOL(ARG(args, 0))->boolean.val)
    return JSUNDEF();
  fh_error(state, E_ASSERTION, "assertion failed");
  UNREACHABLE();
}

js_val *
console_time(js_val *instance, js_args *args, eval_state *state)
{
  // https://developer.mozilla.org/en/DOM/console.time
  // TODO
  return JSUNDEF();
}

js_val *
bootstrap_console()
{
  js_val *console = JSOBJ();

  BUILTIN(console, "log", JSNFUNC(console_log));
  BUILTIN(console, "error", JSNFUNC(console_error));
  BUILTIN(console, "info", JSNFUNC(console_info));
  BUILTIN(console, "assert", JSNFUNC(console_assert));
  BUILTIN(console, "time", JSNFUNC(console_time));

  return console;
}
