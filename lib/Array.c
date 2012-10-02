// Array.c
// -------
// Array properties, methods, and prototype
//
// Known issues:
//  - 'Holes' in arrays are not handled.
//  - 

#include "Array.h"

JSValue *arr_compare_func;

// Array.isArray(obj)
JSValue *
arr_is_array(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = ARG0(args);
  return JSBOOL(obj->type == T_OBJECT && obj->object.is_array);
}

// Array.prototype.pop()
JSValue *
arr_proto_pop(JSValue *instance, JSArgs *args, State *state)
{
  int len = instance->object.length;
  if (len == 0) return JSUNDEF();

  JSValue *key = JSCAST(JSNUM(len - 1), T_STRING);
  JSValue *popped = fh_get(instance, key->string.ptr);

  fh_del_prop(instance, key->string.ptr);
  fh_arr_set_len(instance, len - 1);
  return popped;
}

// Array.prototype.push(element1, ..., elementN)
JSValue *
arr_proto_push(JSValue *instance, JSArgs *args, State *state)
{
  int len = instance->object.length;
  int nargs = ARGLEN(args);
  int i;
  for (i = 0; i<nargs; i++) {
    JSValue *key = JSCAST(JSNUM(len), T_STRING);
    fh_set(instance, key->string.ptr, ARGN(args, i));
    len++;
  }

  fh_arr_set_len(instance, len);
  return JSNUM(len);
}

// Array.prototype.reverse()
JSValue *
arr_proto_reverse(JSValue *instance, JSArgs *args, State *state)
{
  int len = instance->object.length;
  int i = 0, j = len - 1;
  JSValue *ikey, *jkey, *tmp;
  JSProp *iprop, *jprop;

  while(i < j) {
    // While i & j converge, swap the values they point to.
    ikey = JSCAST(JSNUM(i++), T_STRING);
    jkey = JSCAST(JSNUM(j--), T_STRING);
    iprop = fh_get_prop(instance, ikey->string.ptr);
    jprop = fh_get_prop(instance, jkey->string.ptr);
    tmp = iprop->ptr;
    iprop->ptr = jprop->ptr;
    jprop->ptr = tmp;
  }

  return instance;
}

// Array.prototype.shift()
JSValue *
arr_proto_shift(JSValue *instance, JSArgs *args, State *state)
{
  // Shift off first element, but then we have to rename all the keys.
  
  int len = instance->object.length;
  if (len == 0) return JSUNDEF();

  char *key = "0";
  JSValue *shifted = fh_get(instance, key);
  fh_del_prop(instance, key);

  int i;
  for (i=1; i<len; i++) {
    JSValue *old_key = JSCAST(JSNUM(i), T_STRING);
    JSValue *new_key = JSCAST(JSNUM(i - 1), T_STRING);
    JSProp *prop = fh_get_prop(instance, old_key->string.ptr);
    fh_del_prop(instance, old_key->string.ptr);
    fh_set(instance, new_key->string.ptr, prop->ptr);
  }

  fh_arr_set_len(instance, len - 1);
  return shifted;
}

// Array.prototype.sort([compareFunction])
JSValue *
arr_proto_sort(JSValue *instance, JSArgs *args, State *state)
{
  arr_compare_func = ARG0(args);
  if (arr_compare_func)
    HASH_SORT(instance->object.map, arr_custom_sort);
  else
    HASH_SORT(instance->object.map, arr_lex_sort);

  JSProp *p;
  int i = 0;
  JSValue *key = JSCAST(JSNUM(i), T_STRING);

  return instance;
}

// Array.prototype.splice(index, howMany[, element1[, ..., elementN]])
JSValue *
arr_proto_splice(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.unshift(element1, ..., elementN)
JSValue *
arr_proto_unshift(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.concat(value1, value2, ..., valueN)
JSValue *
arr_proto_concat(JSValue *instance, JSArgs *args, State *state)
{
  int nargs = ARGLEN(args);
  int len = instance->object.length;
  JSValue *newarr = JSARR();

  JSValue *arg;
  int i;
  for (i=0; i<nargs; i++) {
    arg = ARGN(args, i);
    if (arg->type == T_OBJECT && arg->object.is_array) {
      // TODO: add all of its items
    }
    else {
      // TODO: just add it
    }
  }
  return newarr;
}

// Array.prototype.join(separator)
JSValue *
arr_proto_join(JSValue *instance, JSArgs *args, State *state)
{
  return arr_do_join(instance, ARG0(args));
}

// Array.prototype.slice(
JSValue *
arr_proto_slice(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.toString()
JSValue *
arr_proto_to_string(JSValue *instance, JSArgs *args, State *state)
{
  return arr_do_join(instance, JSSTR(","));
}

// Array.prototype.indexOf(searchElement[, fromIndex])
JSValue *
arr_proto_index_of(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *search = ARG0(args);
  JSValue *from = ARGN(args, 1);
  int len = instance->object.length;

  int i = 0;
  if (from->type == T_NUMBER) {
    if (from->number.val < 0)
      i = len + from->number.val;
    else
      i = from->number.val;
  }

  JSValue *key, *equals;
  for(; i < instance->object.length && i >= 0; i++) {
    key = JSCAST(JSNUM(i), T_STRING);
    // indexOf uses strict equality
    equals = fh_eq(fh_get(instance, key->string.ptr), search, true);
    if (equals->boolean.val) {
      return JSNUM(i);
    }
  }
  return JSNUM(-1);
}

// Array.prototype.lastIndexOf(searchElement[, fromIndex])
JSValue *
arr_proto_last_index_of(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *search = ARG0(args);
  JSValue *from = ARGN(args, 1);
  int len = instance->object.length;

  int i = len - 1;
  if (from->type == T_NUMBER) {
    if (from->number.val < 0)
      i = len + from->number.val;
    else if (from->number.val < len)
      i = from->number.val;
  }

  JSValue *key, *equals;
  for(; i >= 0; i--) {
    key = JSCAST(JSNUM(i), T_STRING);
    // lastIndexOf uses strict equality
    equals = fh_eq(fh_get(instance, key->string.ptr), search, true);
    if (equals->boolean.val) {
      return JSNUM(i);
    }
  }
  return JSNUM(-1);
}

// Array.prototype.filter(callback[, ctx])
JSValue *
arr_proto_filter(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *callback = ARG0(args);
  JSValue *ctx = ARGN(args, 1);
  if (!ctx) ctx = state->ctx;
  JSValue *filtered = JSARR();
  int len = instance->object.length;

  JSValue *ikey, *jkey, *val, *result;
  JSArgs *cbargs;
  int i = 0, j = 0;
  for (; i<len; i++) {
    ikey = JSCAST(JSNUM(i), T_STRING);
    val = fh_get(instance, ikey->string.ptr);
    cbargs = fh_new_args(val, JSNUM(i), instance);
    result = fh_function_call(ctx, state, callback, cbargs);
    if (JSCAST(result, T_BOOLEAN)->boolean.val) {
      jkey = JSCAST(JSNUM(j++), T_STRING);
      fh_set(filtered, jkey->string.ptr, fh_get(instance, ikey->string.ptr));
    }
  }
  fh_arr_set_len(filtered, j);

  return filtered;
}

// Array.prototype.forEach(callback[, ctx])
JSValue *
arr_proto_for_each(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *callback = ARG0(args);
  JSValue *ctx = ARGN(args, 1);
  if (!ctx) ctx = state->ctx;
  int len = instance->object.length;

  JSValue *key, *val;
  JSArgs *cbargs;
  int i;
  for (i=0; i<len; i++) {
    key = JSCAST(JSNUM(i), T_STRING);
    val = fh_get(instance, key->string.ptr);
    cbargs = fh_new_args(val, JSNUM(i), instance);
    fh_function_call(ctx, state, callback, cbargs);
  }

  return JSUNDEF();
}

// Array.prototype.every(callback[, ctx])
JSValue *
arr_proto_every(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *callback = ARG0(args);
  JSValue *ctx = ARGN(args, 1);
  if (!ctx) ctx = state->ctx;
  int len = instance->object.length;

  JSValue *key, *val, *result;
  JSArgs *cbargs;
  int i;
  for (i = 0; i<len; i++) {
    key = JSCAST(JSNUM(i), T_STRING);
    val = fh_get(instance, key->string.ptr);
    cbargs = fh_new_args(val, JSNUM(i), instance);
    result = fh_function_call(ctx, state, callback, cbargs);
    if (!JSCAST(result, T_BOOLEAN)->boolean.val)
      return JSBOOL(0);
  }

  return JSBOOL(1);
}

// Array.prototype.map(callback[, ctx])
JSValue *
arr_proto_map(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *callback = ARG0(args);
  JSValue *ctx = ARGN(args, 1);
  if (!ctx) ctx = state->ctx;
  int len = instance->object.length;
  JSValue *map = JSARR();

  JSValue *key, *val, *result;
  JSArgs *cbargs;
  int i;
  for (i = 0; i<len; i++) {
    key = JSCAST(JSNUM(i), T_STRING);
    val = fh_get(instance, key->string.ptr);
    cbargs = fh_new_args(val, JSNUM(i), instance);
    result = fh_function_call(ctx, state, callback, cbargs);
    fh_set(map, key->string.ptr, result);
  }

  fh_arr_set_len(map, len);
  return map;
}

// Array.prototype.some(callback[, ctx])
JSValue *
arr_proto_some(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *callback = ARG0(args);
  JSValue *ctx = ARGN(args, 1);
  if (!ctx) ctx = state->ctx;
  int len = instance->object.length;

  JSValue *key, *val, *result;
  JSArgs *cbargs;
  int i;
  for (i = 0; i<len; i++) {
    key = JSCAST(JSNUM(i), T_STRING);
    val = fh_get(instance, key->string.ptr);
    cbargs = fh_new_args(val, JSNUM(i), instance);
    result = fh_function_call(ctx, state, callback, cbargs);
    if (JSCAST(result, T_BOOLEAN)->boolean.val)
      return JSBOOL(1);
  }

  return JSBOOL(0);
}

// Array.prototype.reduce(callback[, seed])
JSValue *
arr_proto_reduce(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *callback = ARG0(args);
  JSValue *ctx = ARGN(args, 1);
  if (!ctx) ctx = state->ctx;
  int len = instance->object.length;
  JSValue *reduction;

  return JSUNDEF();
}

// Array.prototype.reduceRight(callback[, seed])
JSValue *
arr_proto_reduce_right(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

int
arr_lex_sort(JSProp *a, JSProp *b)
{
  JSValue *val_a = JSCAST(a->ptr, T_STRING);
  JSValue *val_b = JSCAST(b->ptr, T_STRING);

  return strcmp(val_a->string.ptr, val_b->string.ptr);
}

int
arr_key_sort(JSProp *a, JSProp *b)
{
  return strcmp(a->name, b->name);
}

int
arr_custom_sort(JSProp *a, JSProp *b)
{
  // TODO
  // JSValue *val_a = JSCAST(a->ptr, T_STRING);
  // JSValue *val_b = JSCAST(b->ptr, T_STRING); 

  // return fh_function_call(arr_custom_sort);
  return 0;
}

JSValue *
arr_do_join(JSValue *arr, JSValue *sep)
{
  JSValue *result = JSSTR("");

  bool first = true;
  JSProp *p;
  JSValue *strval;

  OBJ_ITER(arr, p) {
    if (!p->enumerable) continue;
    if (p->ptr) {
      if (!first)
        result = JSSTR(fh_str_concat(result->string.ptr, sep->string.ptr));
      else
        first = false;
      strval = JSCAST(p->ptr, T_STRING);
      result = JSSTR(fh_str_concat(result->string.ptr, strval->string.ptr));
    }
  }

  return result;
}

JSValue *
bootstrap_array()
{
  JSValue *array = JSOBJ();
  JSValue *proto = JSOBJ();
  proto->object.is_array = true;

  // Array
  // -----

  // Properties
  BUILTIN(array, "prototype", proto);

  // Methods
  BUILTIN(array, "isArray", JSNFUNC(&arr_is_array));

  // Array.prototype
  // ---------------

  // Properties
  // TODO: causing gc problems (infinite loop when marking)
  // fh_set(prototype, "constructor", object);
  BUILTIN(proto, "length", JSNUM(0));

  // Methods
  BUILTIN(proto, "pop", JSNFUNC(&arr_proto_pop));
  BUILTIN(proto, "push", JSNFUNC(&arr_proto_push));
  BUILTIN(proto, "reverse", JSNFUNC(&arr_proto_reverse));
  BUILTIN(proto, "shift", JSNFUNC(&arr_proto_shift));
  BUILTIN(proto, "sort", JSNFUNC(&arr_proto_sort));
  BUILTIN(proto, "splice", JSNFUNC(&arr_proto_splice));
  BUILTIN(proto, "unshift", JSNFUNC(&arr_proto_unshift));

  BUILTIN(proto, "concat", JSNFUNC(&arr_proto_concat));
  BUILTIN(proto, "join", JSNFUNC(&arr_proto_join));
  BUILTIN(proto, "slice", JSNFUNC(&arr_proto_slice));
  BUILTIN(proto, "toString", JSNFUNC(&arr_proto_to_string));
  BUILTIN(proto, "indexOf", JSNFUNC(&arr_proto_index_of));
  BUILTIN(proto, "lastIndexOf", JSNFUNC(&arr_proto_last_index_of));

  BUILTIN(proto, "filter", JSNFUNC(&arr_proto_filter));
  BUILTIN(proto, "forEach", JSNFUNC(&arr_proto_for_each));
  BUILTIN(proto, "every", JSNFUNC(&arr_proto_every));
  BUILTIN(proto, "map", JSNFUNC(&arr_proto_map));
  BUILTIN(proto, "some", JSNFUNC(&arr_proto_some));
  BUILTIN(proto, "reduce", JSNFUNC(&arr_proto_reduce));
  BUILTIN(proto, "reduceRight", JSNFUNC(&arr_proto_reduce_right));

  return array;
}
