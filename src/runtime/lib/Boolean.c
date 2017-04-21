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
  js_val *boolean = JSNFUNC(bool_new, 1);
  js_val *prototype = JSOBJ();

  // Boolean
  // -------

  // Properties
  DEF(boolean, "prototype", prototype);

  // Boolean.prototype
  // -----------------

  // Properties
  DEF(prototype, "constructor", JSNFUNC(bool_new, 1));

  // Methods
  DEF(prototype, "toString", JSNFUNC(bool_proto_to_string, 0));
  DEF(prototype, "valueOf", JSNFUNC(bool_proto_value_of, 0));

  fh_attach_prototype(prototype, fh->function_proto);

  return boolean;
}
