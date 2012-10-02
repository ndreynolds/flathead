// Array.c
// -------
// Array properties, methods, and prototype
//
// Known issues:
//  - 'Holes' in arrays are not handled.
//  - Copies vs. references (slice, concat)

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

  JSValue *key = JSNUMKEY(len-1);
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
    JSValue *key = JSNUMKEY(len);
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
    ikey = JSNUMKEY(i++);
    jkey = JSNUMKEY(j--);
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
    JSValue *old_key = JSNUMKEY(i);
    JSValue *new_key = JSNUMKEY(i-1);
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
  JSValue *key = JSNUMKEY(i);

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
  // Create a new hash map (simplified by creating a new array)
  // to replace into our instance. We're still mutating the array
  // from a high level (no references will be broken).
  
  JSValue *newarr = JSARR();
  int nargs = ARGLEN(args);
  int len = instance->object.length;

  int i = 0;
  int j = 0;

  // Add the args
  for (; i<nargs; i++) {
    fh_set(newarr, JSNUMKEY(i)->string.ptr, ARGN(args, i));
  }

  // Add the instance's elements
  JSValue *val;
  for (; j<len; j++, i++) {
    val = fh_get(instance, JSNUMKEY(j)->string.ptr);
    fh_set(newarr, JSNUMKEY(i)->string.ptr, val);
  }

  // Replace the map into our instance.
  //
  // GC assumes that maps belong to a single object, so we need to delete the
  // second reference, or the hash map will get collected.
  instance->object.map = newarr->object.map;
  newarr->object.map = NULL;

  fh_arr_set_len(instance, i);
  return JSNUM(i);
}

// Array.prototype.concat(value1, value2, ..., valueN)
JSValue *
arr_proto_concat(JSValue *instance, JSArgs *args, State *state)
{
  int nargs = ARGLEN(args);
  int len = instance->object.length;
  JSValue *newarr = JSARR();
  JSValue *key;

  int i = 0; // newarr index
  int j = 0; // args index

  // Add the current array to the new array.
  for (; i<len; i++) {
    key = JSNUMKEY(i);
    fh_set(newarr, key->string.ptr, fh_get(instance, key->string.ptr));
  }

  // Add the arguments to the new array.
  JSValue *arg;
  for (; j<nargs; j++, i++) {
    key = JSNUMKEY(i);
    arg = ARGN(args, j);
    // Extract array elements one level deep.
    if (arg->type == T_OBJECT && arg->object.is_array) {
      int k;
      JSValue *inner_key;
      for (k=0; k<arg->object.length; k++) {
        key = JSNUMKEY(i);
        inner_key = JSNUMKEY(k);
        fh_set(newarr, key->string.ptr, fh_get(arg, inner_key->string.ptr));
        // Let the outer loop increment i if we're at the end:
        if (k < arg->object.length - 1) i++;
      }
    } 
    else {
      fh_set(newarr, key->string.ptr, arg);
    }
  }

  fh_arr_set_len(newarr, i);
  return newarr;
}

// Array.prototype.join(separator)
JSValue *
arr_proto_join(JSValue *instance, JSArgs *args, State *state)
{
  return arr_do_join(instance, ARG0(args));
}

// Array.prototype.slice(begin[, end])
JSValue *
arr_proto_slice(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *begin = ARG0(args);
  JSValue *end = ARGN(args, 1);
  JSValue *slice = JSARR();
  int len = instance->object.length;

  int i = 0;
  int j = begin->number.val;
  int k = end->type != T_UNDEF ? end->number.val : len;

  // Handle negative offsets
  if (j < 0) j = len + j;
  if (k < 0) k = len + k;

  // Correct to array bounds
  if (j < 0) j = 0;
  if (k > len) k = len;

  // slice from j inclusive to k exclusive. 
  JSValue *val;
  for(; j<k && j<len; j++, i++) {
    val = fh_get(instance, JSNUMKEY(j)->string.ptr);
    fh_set(slice, JSNUMKEY(i)->string.ptr, val);
  }

  fh_arr_set_len(slice, i);
  return slice;
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
  for(; i < len && i >= 0; i++) {
    key = JSNUMKEY(i);
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
    key = JSNUMKEY(i);
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
  if (ctx->type == T_UNDEF) ctx = state->ctx;
  JSValue *filtered = JSARR();
  int len = instance->object.length;

  JSValue *ikey, *jkey, *val, *result;
  JSArgs *cbargs;
  int i = 0, j = 0;
  for (; i<len; i++) {
    ikey = JSNUMKEY(i);
    val = fh_get(instance, ikey->string.ptr);
    cbargs = fh_new_args(val, JSNUM(i), instance);
    result = fh_function_call(ctx, state, callback, cbargs);
    if (JSCAST(result, T_BOOLEAN)->boolean.val) {
      jkey = JSNUMKEY(j++);
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
  if (ctx->type == T_UNDEF) ctx = state->ctx;
  int len = instance->object.length;

  JSValue *key, *val;
  JSArgs *cbargs;
  int i;
  for (i=0; i<len; i++) {
    key = JSNUMKEY(i);
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
  if (ctx->type == T_UNDEF) ctx = state->ctx;
  int len = instance->object.length;

  JSValue *key, *val, *result;
  JSArgs *cbargs;
  int i;
  for (i = 0; i<len; i++) {
    key = JSNUMKEY(i);
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
  if (ctx->type == T_UNDEF) ctx = state->ctx;
  int len = instance->object.length;
  JSValue *map = JSARR();

  JSValue *key, *val, *result;
  JSArgs *cbargs;
  int i;
  for (i = 0; i<len; i++) {
    key = JSNUMKEY(i);
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
  if (ctx->type == T_UNDEF) ctx = state->ctx;
  int len = instance->object.length;

  JSValue *key, *val, *result;
  JSArgs *cbargs;
  int i;
  for (i = 0; i<len; i++) {
    key = JSNUMKEY(i);
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
  JSValue *reduction = ARGN(args, 1);
  JSValue *ctx = state->ctx;
  int len = instance->object.length;
  int i = 0;

  if (reduction->type == T_UNDEF) { 
    reduction = fh_get(instance, "0");
    i = 1;
  }

  JSValue *key, *val;
  JSArgs *cbargs;
  for (; i<len; i++) {
    key = JSNUMKEY(i);
    val = fh_get(instance, key->string.ptr);
    cbargs = fh_new_args(reduction, val, JSNUM(i));
    reduction = fh_function_call(ctx, state, callback, cbargs);
  }

  return reduction;
}

// Array.prototype.reduceRight(callback[, seed])
JSValue *
arr_proto_reduce_right(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *callback = ARG0(args);
  JSValue *reduction = ARGN(args, 1);
  JSValue *ctx = state->ctx;
  int len = instance->object.length;
  int i = len - 1;

  if (reduction->type == T_UNDEF) { 
    reduction = fh_get(instance, JSNUMKEY(len-1)->string.ptr);
    i = len - 2;
  }

  JSValue *key, *val;
  JSArgs *cbargs;
  for (; i>=0; i--) {
    key = JSNUMKEY(i);
    val = fh_get(instance, key->string.ptr);
    cbargs = fh_new_args(reduction, val, JSNUM(i));
    reduction = fh_function_call(ctx, state, callback, cbargs);
  }

  return reduction;
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
  JSValue *proto = JSARR();

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
