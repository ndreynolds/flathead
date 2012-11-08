// Boolean.c
// ---------
// Boolean properties, methods, and prototype

#include "Boolean.h"

// (new) Boolean(value)
js_val *
bool_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *value = ARG(args, 0);
  if (state->construct)
    state->this->object.primitive = TO_BOOL(value);
  return TO_BOOL(value);
}

// Boolean.prototype.toString()
js_val *
bool_proto_to_string(js_val *instance, js_args *args, eval_state *state)
{
  return instance->boolean.val ? JSSTR("true") : JSSTR("false");
}

// Boolean.prototype.valueOf()
js_val *
bool_proto_value_of(js_val *instance, js_args *args, eval_state *state)
{
  return instance;
}

js_val *
bootstrap_boolean()
{
  js_val *boolean = JSNFUNC(bool_new);
  js_val *prototype = JSOBJ();

  // Boolean
  // -------

  // Properties
  BUILTIN(boolean, "prototype", prototype);

  // Boolean.prototype
  // -----------------
  
  // Properties
  BUILTIN(prototype, "constructor", JSNFUNC(bool_new));
  
  // Methods
  BUILTIN(prototype, "toString", JSNFUNC(bool_proto_to_string));
  BUILTIN(prototype, "valueOf", JSNFUNC(bool_proto_value_of));

  return boolean;
}
