// String.c
// ----------
// String properties, methods, and prototype

#include "String.h"

JSValue *
str_new(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_from_char_code(JSValue *instance, JSArgs *args, State *state)
{
  fh_error(state, E_EVAL, "Unicode is not supported");
  return JSUNDEF();
}

// String.prototype.charAt(index)
JSValue *
str_proto_char_at(JSValue *instance, JSArgs *args, State *state)
{
  int index = JSCAST(ARG0(args), T_NUMBER)->number.val;

  if (index < 0 || index >= instance->string.length)
    return JSSTR("");
  char *str = malloc(2);
  sprintf(str, "%c", instance->string.ptr[index]);
  return JSSTR(str);
}

// String.prototype.charCodeAt(index)
JSValue *
str_proto_char_code_at(JSValue *instance, JSArgs *args, State *state)
{
  fh_error(state, E_EVAL, "Unicode is not supported");
  return JSUNDEF();
}

// String.prototype.concat(string2, string3[, ..., stringN])
JSValue *
str_proto_concat(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *new_str = JSSTR(instance->string.ptr);

  JSValue *arg;
  int i;
  for (i=0; i<ARGLEN(args); i++) {
    arg = JSCAST(ARGN(args, i), T_STRING);
    printf("%s : %s\n", new_str->string.ptr, arg->string.ptr);
    fh_str_concat(new_str->string.ptr, arg->string.ptr);
  }

  new_str->string.length = strlen(new_str->string.ptr);
  return new_str;
}

JSValue *
str_proto_index_of(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_last_index_of(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_locale_compare(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_match(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_quote(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_replace(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_search(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_slice(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_split(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_substr(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_substring(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_to_locale_lower_case(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_to_locale_upper_case(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_to_lower_case(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_to_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_to_upper_case(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_trim(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_trim_left(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_trim_right(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
str_proto_value_of(JSValue *instance, JSArgs *args, State *state)
{
  return instance;
}

JSValue *
bootstrap_string()
{
  JSValue *string = JSOBJ();
  JSValue *prototype = JSOBJ();

  // String
  // ------

  BUILTIN(string, "prototype", prototype);

  // String.prototype
  // ----------------

  // Properties

  BUILTIN(prototype, "constructor", JSNFUNC(&str_new)); 
  BUILTIN(prototype, "length", JSNUM(0));

  // Methods
  
  BUILTIN(prototype, "charAt", JSNFUNC(&str_proto_char_at));
  BUILTIN(prototype, "charCodeAt", JSNFUNC(&str_proto_char_code_at));
  BUILTIN(prototype, "concat", JSNFUNC(&str_proto_concat));
  BUILTIN(prototype, "indexOf", JSNFUNC(&str_proto_index_of));
  BUILTIN(prototype, "lastIndexOf", JSNFUNC(&str_proto_last_index_of));
  BUILTIN(prototype, "localeCompare", JSNFUNC(&str_proto_locale_compare));
  BUILTIN(prototype, "match", JSNFUNC(&str_proto_match));
  BUILTIN(prototype, "quote", JSNFUNC(&str_proto_quote));
  BUILTIN(prototype, "replace", JSNFUNC(&str_proto_replace));
  BUILTIN(prototype, "search", JSNFUNC(&str_proto_search));
  BUILTIN(prototype, "slice", JSNFUNC(&str_proto_slice));
  BUILTIN(prototype, "split", JSNFUNC(&str_proto_split));
  BUILTIN(prototype, "substr", JSNFUNC(&str_proto_substr));
  BUILTIN(prototype, "substring", JSNFUNC(&str_proto_substring));
  BUILTIN(prototype, "toLocaleLowerCase", JSNFUNC(&str_proto_to_locale_lower_case));
  BUILTIN(prototype, "toLocaleUpperCase", JSNFUNC(&str_proto_to_locale_upper_case));
  BUILTIN(prototype, "toLowerCase", JSNFUNC(&str_proto_to_lower_case));
  BUILTIN(prototype, "toString", JSNFUNC(&str_proto_to_string));
  BUILTIN(prototype, "toUpperCase", JSNFUNC(&str_proto_to_upper_case));
  BUILTIN(prototype, "trim", JSNFUNC(&str_proto_trim));
  BUILTIN(prototype, "trimLeft", JSNFUNC(&str_proto_trim_left));
  BUILTIN(prototype, "trimRight", JSNFUNC(&str_proto_trim_right));
  BUILTIN(prototype, "valueOf", JSNFUNC(&str_proto_value_of));

  return string;
}
