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

// [new] EvalError(message)
js_val *
error_eval_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *err = error_new(instance, args, state);
  fh_set(err, "name", JSSTR(E_EVAL));
  err->proto = fh_try_get_proto(E_EVAL);
  return err;
}

// [new] RangeError(message)
js_val *
error_range_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *err = error_new(instance, args, state);
  fh_set(err, "name", JSSTR(E_RANGE));
  err->proto = fh_try_get_proto(E_RANGE);
  return err;
}

// [new] ReferenceError(message)
js_val *
error_ref_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *err = error_new(instance, args, state);
  fh_set(err, "name", JSSTR(E_REFERENCE));
  err->proto = fh_try_get_proto(E_REFERENCE);
  return err;
}

// [new] SyntaxError(message)
js_val *
error_syntax_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *err = error_new(instance, args, state);
  fh_set(err, "name", JSSTR(E_SYNTAX));
  err->proto = fh_try_get_proto(E_SYNTAX);
  return err;
}

// [new] TypeError(message)
js_val *
error_type_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *err = error_new(instance, args, state);
  fh_set(err, "name", JSSTR(E_TYPE));
  err->proto = fh_try_get_proto(E_TYPE);
  return err;
}

// [new] URIError(message)
js_val *
error_uri_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *err = error_new(instance, args, state);
  fh_set(err, "name", JSSTR(E_URI));
  err->proto = fh_try_get_proto(E_URI);
  return err;
}

// Error.prototype.toString()
js_val *
error_proto_to_string(js_val *instance, js_args *args, eval_state *state)
{
  if (!IS_OBJ(instance))
    fh_throw(state,
        fh_new_error(E_TYPE, "Error.prototype.toString called on non-object"));

  js_val *name_prop = fh_get_proto(instance, "name");
  js_val *msg_prop = fh_get_proto(instance, "message");
  js_val *name = IS_UNDEF(name_prop) ? JSSTR("Error") : TO_STR(name_prop);
  js_val *msg = IS_UNDEF(msg_prop) ? JSSTR("") : TO_STR(msg_prop);

  if (strlen(name->string.ptr) == 0) return msg;
  if (strlen(msg->string.ptr) == 0) return name;
  return JSSTR(fh_str_concat(fh_str_concat(name->string.ptr, ": "), msg->string.ptr));
}

js_val *
bootstrap_error(js_val *global)
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


  // Other Error constructors
  // ------------------------

  // EvalError
  js_val *error_eval = JSNFUNC(error_eval_new, 1);
  js_val *error_eval_proto = JSOBJ();
  error_eval_proto->proto = prototype;
  DEF(error_eval, "prototype", error_eval_proto);
  DEF(global, "EvalError", error_eval);

  // RangeError
  js_val *error_range = JSNFUNC(error_range_new, 1);
  js_val *error_range_proto = JSOBJ();
  error_range_proto->proto = prototype;
  DEF(error_range, "prototype", error_range_proto);
  DEF(global, "RangeError", error_range);

  // ReferenceError
  js_val *error_ref = JSNFUNC(error_ref_new, 1);
  js_val *error_ref_proto = JSOBJ();
  error_ref_proto->proto = prototype;
  DEF(error_ref, "prototype", error_ref_proto);
  DEF(global, "ReferenceError", error_ref);

  // SyntaxError
  js_val *error_syntax = JSNFUNC(error_syntax_new, 1);
  js_val *error_syntax_proto = JSOBJ();
  error_syntax_proto->proto = prototype;
  DEF(error_syntax, "prototype", error_syntax_proto);
  DEF(global, "SyntaxError", error_syntax);

  // TypeError
  js_val *error_type = JSNFUNC(error_type_new, 1);
  js_val *error_type_proto = JSOBJ();
  error_type_proto->proto = prototype;
  DEF(error_type, "prototype", error_type_proto);
  DEF(global, "TypeError", error_type);

  // URIError
  js_val *error_uri = JSNFUNC(error_uri_new, 1);
  js_val *error_uri_proto = JSOBJ();
  error_uri_proto->proto = prototype;
  DEF(error_uri, "prototype", error_uri_proto);
  DEF(global, "URIError", error_uri);

  return error;
}
