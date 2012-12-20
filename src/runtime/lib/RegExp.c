// RegExp.c
// --------

#include "RegExp.h"

// [new] RegExp(pattern)
js_val *
regexp_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *pattern = ARG(args, 0);
  js_val *flags = ARG(args, 1);

  js_val *regexp = JSOBJ();

  fh_set_class(regexp, "RegExp");
  fh_set(regexp, "source", IS_UNDEF(pattern) ? JSSTR("(?:)") : TO_STR(pattern));

  if (ARGLEN(args) <= 1)
    return regexp;
  if (!IS_STR(flags))
    fh_error(state, E_TYPE, "Invalid flags supplied to RegExp constructor");
  int i, n;
  for (i = 0, n = strlen(flags->string.ptr); i < n; i++) {
    switch(flags->string.ptr[i]) {
      case 'g': fh_set(regexp, "global", JSBOOL(1)); break;
      case 'i': fh_set(regexp, "ignoreCase", JSBOOL(1)); break;
      case 'm': fh_set(regexp, "multiline", JSBOOL(1)); break;
      case 'y': fh_set(regexp, "sticky", JSBOOL(1)); break;
    }
  }

  regexp->proto = fh_try_get_proto("RegExp");
  return regexp;
}

// RegExp.prototype.exec(str)
js_val *
regexp_proto_exec(js_val *instance, js_args *args, eval_state *state)
{
  js_val *pattern = fh_get_proto(instance, "source"),
         *last_ind = fh_get_proto(instance, "lastIndex"),
         *str = ARG(args, 0),
         *g = fh_get_proto(instance, "global"),
         *c = fh_get_proto(instance, "ignoreCase"),
         *m = fh_get_proto(instance, "multiline");

  bool matched = false;
  bool caseless = c->boolean.val;
  int *matches;
  int count;
  int length = strlen(str->string.ptr);
  int i = fh_to_int32(last_ind)->number.val;
  if (!TO_BOOL(g)->boolean.val)
    i = 0;

  while (!matched) {
    if (i < 0 || i > length) {
      fh_set(instance, "lastIndex", JSNUM(0));
      return JSNULL();
    }
    matches = fh_regexp(str->string.ptr, pattern->string.ptr, &count, 0, caseless);
    if (count == 0) 
      i++;
    else
      matched = true;
  }

  fh_set(instance, "lastIndex", JSNUM(i));

  js_val *res = JSOBJ();
  js_val *arr = JSARR();
  fh_set(instance, "index", JSNUM(i));
  fh_set(instance, "input", str);
}

// RegExp.prototype.test([str])
js_val *
regexp_proto_test(js_val *instance, js_args *args, eval_state *state)
{
  char *str = TO_STR(ARG(args, 0))->string.ptr;
  char *pattern = TO_STR(fh_get(instance, "source"))->string.ptr;
  bool caseless = fh_get_proto(instance, "ignoreCase")->boolean.val;
  int count;
  fh_regexp(str, pattern, &count, 0, caseless);
  return JSBOOL(count > 0);
}

// RegExp.prototype.toString()
js_val *
regexp_proto_to_string(js_val *instance, js_args *args, eval_state *state)
{
  js_val *pattern = fh_get_proto(instance, "source"),
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

  js_val *res = JSSTR(new);
  free(new);
  return res;
}

js_val *
bootstrap_regexp()
{
  js_val *regexp = JSNFUNC(regexp_new, 2);
  js_val *prototype = JSOBJ();
  prototype->proto = fh->object_proto;

  // RegExp
  // ------

  // Properties
  DEF(regexp, "prototype", prototype);

  // RegExp.prototype
  // ----------------

  // Properties
  DEF(prototype, "constructor", JSNFUNC(regexp_new, 2));
  DEF2(prototype, "global", JSBOOL(0), P_NONE);
  DEF2(prototype, "ignoreCase", JSBOOL(0), P_NONE);
  DEF2(prototype, "lastIndex", JSBOOL(0), P_WRITE);
  DEF2(prototype, "multiline", JSBOOL(0), P_NONE);
  DEF2(prototype, "source", JSSTR(""), P_NONE);
  DEF2(prototype, "sticky", JSBOOL(0), P_NONE);

  // Methods
  DEF(prototype, "exec", JSNFUNC(regexp_proto_exec, 1));
  DEF(prototype, "test", JSNFUNC(regexp_proto_test, 1));
  DEF(prototype, "toString", JSNFUNC(regexp_proto_to_string, 0));

  fh_attach_prototype(prototype, fh->function_proto);

  return regexp;
}
