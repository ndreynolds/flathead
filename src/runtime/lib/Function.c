// Function.c
// ----------
// Function properties, methods, and prototype

#include "Function.h"

// new Function([arg1[, arg2[, ...]],] functionBody)
js_val *
func_new(js_val *instance, js_args *args, eval_state *state)
{
  // TODO: No clue how this is going to work. Need to generate
  // nodes for the args, which can be manual, but also the func
  // body, which we'll need the parser's help with.
  return JSUNDEF();
}

// Function.prototype.apply(thisValue[, argsArray])
js_val *
func_proto_apply(js_val *instance, js_args *args, eval_state *state)
{
  js_val *this = ARG(args, 0);
  js_val *arr = ARG(args, 1);
  js_args *func_args = malloc(sizeof(js_args));
  js_args *func_args_head = func_args;

  int len = arr->object.length;
  int i;

  for (i = 0; i < len; i++) {
    func_args->arg = fh_get(arr, JSNUMKEY(i)->string.ptr);
    func_args->next = NULL;
    if (i < len - 1) {
      func_args->next = malloc(sizeof(js_args));
      func_args = func_args->next;
    }
  }

  return fh_call(state->ctx, this, state, instance, func_args_head);
}

// Function.prototype.apply(thisValue[, arg1[, arg2[, ...]]])
js_val *
func_proto_bind(js_val *instance, js_args *args, eval_state *state)
{
  js_val *this = ARG(args, 0);

  // Shift off the first argument.
  args->arg = NULL;
  if (args->next) 
    args = args->next;

  js_val *func = JSFUNC(instance->object.node);
  func->object.bound_this = this;
  func->object.bound_args = args;
  return func;
}

// Function.prototype.call(thisValue[, arg1[, arg2[, ...]]])
js_val *
func_proto_call(js_val *instance, js_args *args, eval_state *state)
{
  js_val *this = ARG(args, 0);
  args->arg = NULL;
  if (args->next)
    args = args->next;
  return fh_call(state->ctx, this, state, instance, args);
}

// Function.prototype.isGenerator()
js_val *
func_proto_is_generator(js_val *instance, js_args *args, eval_state *state)
{
  return JSBOOL(instance->object.generator);
}

js_val *
func_proto_to_string(js_val *instance, js_args *args, eval_state *state)
{
  // TODO: Need to serialize nodes to string.
  return JSSTR("[Function function]");
}

js_val *
bootstrap_function()
{
  js_val *function = JSNFUNC(func_new);
  js_val *prototype = JSFUNC(NULL);

  // Function
  // --------

  BUILTIN(function, "prototype", prototype);

  // Function.prototype
  // ------------------

  // Properties
  BUILTIN(prototype, "length", JSNUM(0));
  BUILTIN(prototype, "constructor", JSNFUNC(func_new));

  // Methods
  BUILTIN(prototype, "apply", JSNFUNC(func_proto_apply));
  BUILTIN(prototype, "bind", JSNFUNC(func_proto_bind));
  BUILTIN(prototype, "call", JSNFUNC(func_proto_call));
  BUILTIN(prototype, "isGenerator", JSNFUNC(func_proto_is_generator));
  BUILTIN(prototype, "toString", JSNFUNC(func_proto_to_string));

  fh->function_proto = prototype;

  return function;
}
