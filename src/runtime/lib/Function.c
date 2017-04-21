// Function.c
// ----------
// Function properties, methods, and prototype

#include "Function.h"

// new Function([arg1[, arg2[, ...]],] functionBody)
js_val *
func_new(js_val *instance, js_args *args, eval_state *state)
{
  unsigned arglen = ARGLEN(args);
  unsigned i = 0;
  char *arg_lst = NULL;
  char *tmp;

  for (i = 0; arglen > 0 && i < (arglen - 1); i++) {
    if (!arg_lst) {
      arg_lst = TO_STR(ARG(args, i))->string.ptr;
    }
    else {
      tmp = arg_lst;
      arg_lst = fh_str_concat(arg_lst, ", ");
      free(tmp);
      tmp = arg_lst;
      arg_lst = fh_str_concat(arg_lst, TO_STR(ARG(args, i))->string.ptr);
      free(tmp);
    }
  }

  if (arglen <= 1)
    arg_lst = "";

  char *body = arglen > 0 ? TO_STR(ARG(args, i))->string.ptr : "";
  char *fmt = "(function(%s) { %s });";

  int size = snprintf(NULL, 0, fmt, arg_lst, body) + 1;
  char *func_def = malloc(size);
  snprintf(func_def, size, fmt, arg_lst, body);

  return fh_eval_string(func_def, state->ctx);
}

// Function.prototype.apply(thisValue[, argsArray])
js_val *
func_proto_apply(js_val *instance, js_args *args, eval_state *state)
{
  js_val *this = ARG(args, 0);
  js_val *arr = ARG(args, 1);

  js_args *func_args = args_new();

  unsigned long i;
  for (i = 0; i < arr->object.length; i++)
    args_append(func_args, fh_get(arr, JSNUMKEY(i)->string.ptr));

  return fh_call(state->ctx, this, instance, func_args);
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

  return fh_call(state->ctx, this, instance, args);
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
  js_val *function = JSNFUNC(func_new, 1);
  js_val *prototype = JSFUNC(NULL);
  function->proto = prototype;
  prototype->proto = fh->object_proto;

  // Function
  // --------

  DEF(function, "prototype", prototype);

  // Function.prototype
  // ------------------

  // Properties
  DEF(prototype, "length", JSNUM(0));
  DEF(prototype, "constructor", JSNFUNC(func_new, 1));

  // Methods
  DEF(prototype, "apply", JSNFUNC(func_proto_apply, 2));
  DEF(prototype, "bind", JSNFUNC(func_proto_bind, 1));
  DEF(prototype, "call", JSNFUNC(func_proto_call, 1));
  DEF(prototype, "isGenerator", JSNFUNC(func_proto_is_generator, 0));
  DEF(prototype, "toString", JSNFUNC(func_proto_to_string, 0));

  fh->function_proto = prototype;

  return function;
}
