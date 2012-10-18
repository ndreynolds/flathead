// RegExp.c
// --------

#include "RegExp.h"

// new RegExp(pattern)
JSValue *
regexp_new(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// RegExp.prototype.exec(str)
JSValue *
regexp_proto_exec(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// RegExp.prototype.test([str])
JSValue *
regexp_proto_test(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// RegExp.prototype.toString()
JSValue *
regexp_proto_to_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
bootstrap_regexp()
{
  JSValue *regexp = JSOBJ();
  JSValue *prototype = JSOBJ();

  // RegExp
  // ------

  // Properties
  BUILTIN(regexp, "prototype", prototype);

  // RegExp.prototype
  // ----------------

  // Properties
  // FIXME: these properties need to be non-writable.
  BUILTIN(prototype, "constructor", JSNFUNC(&regexp_new));
  BUILTIN(prototype, "global", JSBOOL(0));
  BUILTIN(prototype, "ignoreCase", JSBOOL(0));
  BUILTIN(prototype, "lastIndex", JSBOOL(0));
  BUILTIN(prototype, "multiline", JSBOOL(0));
  BUILTIN(prototype, "source", JSSTR(""));

  // Methods
  BUILTIN(prototype, "exec", JSNFUNC(&regexp_proto_exec));
  BUILTIN(prototype, "test", JSNFUNC(&regexp_proto_test));
  BUILTIN(prototype, "toString", JSNFUNC(&regexp_proto_to_string));

  return regexp;
}
