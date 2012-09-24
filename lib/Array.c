// Array.c
// -------
// Array properties, methods, and prototype

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
  int i;
  for (i=0; i<len; i++) {
    JSValue *old_key = JSCAST(JSNUM(i), T_STRING);
    JSValue *new_key = JSCAST(JSNUM(len - i + 1), T_STRING);
    JSProp *prop = fh_get_prop(instance, old_key->string.ptr);
    prop->name = new_key->string.ptr;
  }

  // keys are now correct, but the order is not.
  HASH_SORT(instance->object.map, arr_key_sort);

  return JSUNDEF();
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
    prop->name = new_key->string.ptr;
  }

  fh_arr_set_len(instance, len - 1);
  return shifted;
}

// Array.prototype.sort([compareFunction])
JSValue *
arr_proto_sort(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *func = ARG0(args);
  if (func) {
    arr_compare_func = func;
    HASH_SORT(instance->object.map, arr_custom_sort);
  }
  else {
    HASH_SORT(instance->object.map, arr_lex_sort);
  }
  return instance;
}

// Array.prototype.splice(
JSValue *
arr_proto_splice(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.unshift(arr)
JSValue *
arr_proto_unshift(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.concat(
JSValue *
arr_proto_concat(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.join(
JSValue *
arr_proto_join(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.slice(
JSValue *
arr_proto_slice(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.toString(
JSValue *
arr_proto_to_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.indexOf(
JSValue *
arr_proto_index_of(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.lastIndexOf(
JSValue *
arr_proto_last_index_of(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.filter(
JSValue *
arr_proto_filter(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.forEach(
JSValue *
arr_proto_for_each(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.every(
JSValue *
arr_proto_every(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.map(
JSValue *
arr_proto_map(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.some(
JSValue *
arr_proto_some(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.reduce(
JSValue *
arr_proto_reduce(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Array.prototype.reduceRight(
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
