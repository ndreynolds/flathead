// Function.c
// ----------
// Function properties, methods, and prototype

#include "Function.h"

// new Function([arg1[, arg2[, ...]],] functionBody)
JSValue *
func_new(JSValue *instance, JSArgs *args, State *state)
{
  // TODO: No clue how this is going to work. Need to generate
  // nodes for the args, which can be manual, but also the func
  // body, which we'll need the parser's help with.
  return JSUNDEF();
}

// Function.prototype.apply(thisValue[, argsArray])
JSValue *
func_proto_apply(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *this = ARG0(args);
  // TODO: Incorrect, need to transform array into args.
  args->arg = NULL;
  if (args->next) 
    args = args->next;
  return fh_function_call(this, state, instance, args);
}

// Function.prototype.apply(thisValue[, arg1[, arg2[, ...]]])
JSValue *
func_proto_bind(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *this = ARG0(args);
  args->arg = NULL;
  if (args->next) 
    args = args->next;

  JSValue *func = JSFUNC(instance->function.node);
  func->function.bound_this = this;
  func->function.bound_args = args;
  return func;
}

// Function.prototype.call(thisValue[, arg1[, arg2[, ...]]])
JSValue *
func_proto_call(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *this = ARG0(args);
  args->arg = NULL;
  if (args->next) 
    args = args->next;
  return fh_function_call(this, state, instance, args);
}

// Function.prototype.isGenerator()
JSValue *
func_proto_is_generator(JSValue *instance, JSArgs *args, State *state)
{
  return JSBOOL(instance->function.is_generator);
}

JSValue *
func_proto_to_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO: Need to serialize nodes to string.
  return JSSTR("[Function function]");
}

JSValue *
bootstrap_function()
{
  JSValue *function = JSOBJ();
  JSValue *prototype = JSOBJ();

  // Function
  // --------

  BUILTIN(function, "prototype", prototype);

  // Function.prototype
  // ------------------

  // Properties
  
  BUILTIN(prototype, "length", JSNUM(0));
  BUILTIN(prototype, "constructor", JSNFUNC(&func_new));

  // Methods

  BUILTIN(prototype, "apply", JSNFUNC(&func_proto_apply));
  BUILTIN(prototype, "bind", JSNFUNC(&func_proto_bind));
  BUILTIN(prototype, "call", JSNFUNC(&func_proto_call));
  BUILTIN(prototype, "isGenerator", JSNFUNC(&func_proto_is_generator));
  BUILTIN(prototype, "toString", JSNFUNC(&func_proto_to_string));

  return function;
}
