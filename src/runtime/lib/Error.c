// Error.c
// -------

#include "Error.h"


// [new] Error(message)
js_val *
error_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *err = JSOBJ();
  js_val *msg = ARG(args, 0);
  fh_set_class(err, "Error");

  if (!IS_UNDEF(msg))
    fh_set(err, "message", TO_STR(msg));

  err->proto = fh_try_get_proto("Error");
  return err;
}

// Error.prototype.toString()
js_val *
error_proto_to_string(js_val *instance, js_args *args, eval_state *state)
{
  if (!IS_OBJ(instance))
    fh_error(state, E_TYPE, "Error.prototype.toString called on a non-object");

  js_val *name_prop = fh_get_proto(instance, "name");
  js_val *msg_prop = fh_get_proto(instance, "message");
  js_val *name = IS_UNDEF(name_prop) ? JSSTR("Error") : TO_STR(name_prop);
  js_val *msg = IS_UNDEF(msg_prop) ? JSSTR("") : TO_STR(msg_prop);

  if (strlen(name->string.ptr) == 0) return msg;
  if (strlen(msg->string.ptr) == 0) return name;
  return JSSTR(fh_str_concat(fh_str_concat(name->string.ptr, ": "), msg->string.ptr));
}

js_val *
bootstrap_error()
{
  js_val *error = JSNFUNC(error_new, 1);
  js_val *prototype = JSOBJ();
  prototype->proto = fh->object_proto;

  // Error
  // -----

  // Properties
  DEF(error, "prototype", prototype);

  // Error.prototype
  // ---------------
  
  // Properties
  DEF(prototype, "constructor", JSNFUNC(error_new, 1));
  DEF(prototype, "name", JSSTR("Error"));
  DEF(prototype, "message", JSSTR(""));

  // Methods
  DEF(prototype, "toString", JSNFUNC(error_proto_to_string, 0));

  fh_attach_prototype(prototype, fh->function_proto);

  return error;
}
