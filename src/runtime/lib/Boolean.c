// Boolean.c
// ---------
// Boolean properties, methods, and prototype

#include "Boolean.h"

// new Boolean(value)
JSValue *
bool_new(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Boolean.prototype.toString()
JSValue *
bool_proto_to_string(JSValue *instance, JSArgs *args, State *state)
{
  return instance->boolean.val ? JSSTR("true") : JSSTR("false");
}

// Boolean.prototype.valueOf()
JSValue *
bool_proto_value_of(JSValue *instance, JSArgs *args, State *state)
{
  return instance;
}

JSValue *
bootstrap_boolean()
{
  JSValue *boolean = JSOBJ();
  JSValue *prototype = JSOBJ();

  // Boolean

  BUILTIN(boolean, "prototype", prototype);

  // Boolean.prototype
  
  BUILTIN(prototype, "toString", JSNFUNC(&bool_proto_to_string));
  BUILTIN(prototype, "valueOf", JSNFUNC(&bool_proto_value_of));

  return boolean;
}
