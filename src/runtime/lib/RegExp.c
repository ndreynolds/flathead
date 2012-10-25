// RegExp.c
// --------

#include "RegExp.h"

// new RegExp(pattern)
JSValue *
regexp_new(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *pattern = ARG(args, 0);
  JSValue *flags = ARG(args, 1);

  JSValue *regexp = JSOBJ();
  regexp->object.is_regexp = true;
  regexp->proto = fh_try_get_proto("RegExp");

  // TODO: Make a copy of pattern??
  fh_set(regexp, "source", IS_UNDEF(pattern) ? JSSTR("(?:)") : TO_STR(pattern));

  if (ARGLEN(args) <= 1)
    return regexp;
  if (!IS_STR(flags))
    fh_error(state, E_TYPE, "Invalid flags supplied to RegExp constructor");
  int i;
  for (i = 0; i < strlen(flags->string.ptr); i++) {
    switch(flags->string.ptr[i]) {
      case 'g': fh_set(regexp, "global", JSBOOL(1)); break;
      case 'i': fh_set(regexp, "ignoreCase", JSBOOL(1)); break;
      case 'm': fh_set(regexp, "multiline", JSBOOL(1)); break;
      case 'y': fh_set(regexp, "sticky", JSBOOL(1)); break;
    }
  }
  return regexp;
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
  JSValue *pattern = fh_get_proto(instance, "source"),
          *g = fh_get_proto(instance, "global"),
          *i = fh_get_proto(instance, "ignoreCase"),
          *m = fh_get_proto(instance, "multiline"),
          *y = fh_get_proto(instance, "sticky");

  size_t size = strlen(pattern->string.ptr) + 7; // space for 2 slashes and imgy
  char *new = malloc(size);
  snprintf(
    new,
    size,
    "/%s/%s%s%s%s",
    pattern->string.ptr,
    g->boolean.val ? "g" : "",
    i->boolean.val ? "i" : "",
    m->boolean.val ? "m" : "",
    y->boolean.val ? "y" : ""
  );

  JSValue *res = JSSTR(new);
  free(new);
  return res;
}

JSValue *
bootstrap_regexp()
{
  JSValue *regexp = JSNFUNC(&regexp_new);
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
  BUILTIN(prototype, "sticky", JSBOOL(0));

  // Methods
  BUILTIN(prototype, "exec", JSNFUNC(&regexp_proto_exec));
  BUILTIN(prototype, "test", JSNFUNC(&regexp_proto_test));
  BUILTIN(prototype, "toString", JSNFUNC(&regexp_proto_to_string));

  return regexp;
}
