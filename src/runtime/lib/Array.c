// Array.c
// -------
// Array properties, methods, and prototype
//
// Known issues:
//  - 'Holes' in arrays are not handled.
//  - Copies vs. references (slice, concat)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Array.h"


// ---------------------------------------------------------------------------- 
// Merge Sort (implements Array#sort)
// ---------------------------------------------------------------------------- 

static int (*cmp_func)(js_prop *, js_prop *);   // Current Array#sort cmp func
static js_val *js_cmp_func;                     // JavaScript-defined cmp func
static eval_state *js_cmp_state;                // Eval state of JS cmp func

static void 
merge(js_prop **left, js_prop **right, 
          unsigned long l_len, unsigned long r_len, js_prop **out)
{
  unsigned long i, j, k;
  for (i = j = k = 0; i < l_len && j < r_len; )
    out[k++] = cmp_func(left[i], right[j]) ? left[i++] : right[j++];

  while (i < l_len) out[k++] = left[i++];
  while (j < r_len) out[k++] = right[j++];
}
 
static void 
recur(js_prop **arr, js_prop **tmp, unsigned long len)
{
  long l = len / 2;
  if (len <= 1) return;

  recur(tmp, arr, l);
  recur(tmp + l, arr + l, len - l);

  merge(tmp, tmp + l, l, len - l, arr);
}
 
static void 
merge_sort(js_prop **arr, unsigned long len)
{
  js_prop **tmp = malloc(sizeof(js_prop *) * len);
  memcpy(tmp, arr, sizeof(js_prop *) * len);

  recur(arr, tmp, len);

  free(tmp);
}

static int
cmp(js_prop *a, js_prop *b)
{
  return strcmp(TO_STR(a->ptr)->string.ptr, TO_STR(b->ptr)->string.ptr) < 0;
}

static int
cmp_js(js_prop *a, js_prop *b)
{
  js_args *args = fh_new_args(a->ptr, b->ptr, 0);
  eval_state *call_state = fh_new_state(js_cmp_state->line, js_cmp_state->column);
  fh_push_state(call_state);
  js_val *result = fh_call(js_cmp_state->ctx, JSUNDEF(), call_state, js_cmp_func, args);
  return TO_BOOL(result)->number.val > 0;
}


// ---------------------------------------------------------------------------- 
// Array Constructor
// ---------------------------------------------------------------------------- 

// new Array(element0, element1, ..., elementN)
// new Array(arrayLength)
js_val *
arr_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *arr = JSARR();

  // Create array of given length:
  if (ARGLEN(args) == 1 && IS_NUM(ARG(args, 0))) {
    double len = ARG(args, 0)->number.val;
    // Must be positive integer less than 2^32 - 1
    if (len < 0 || len >= ULONG_MAX || fmod(len, 1) != 0)
      fh_error(state, E_RANGE, "Invalid array length");

    fh_set_len(arr, len);
    return arr;
  }

  // Create array of elements

  unsigned int i; // num of args will be less than UINT_MAX
  for (i = 0; i < ARGLEN(args); i++) {
    fh_set(arr, JSNUMKEY(i)->string.ptr, ARG(args, i));
  }
  fh_set_len(arr, i);
  return arr;
}


// ---------------------------------------------------------------------------- 
// Array Methods
// ---------------------------------------------------------------------------- 

// Array.isArray(obj)
js_val *
arr_is_array(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = ARG(args, 0);
  return JSBOOL(IS_ARR(obj));
}


// ---------------------------------------------------------------------------- 
// Array Prototype
// ---------------------------------------------------------------------------- 

// Array.prototype.pop()
js_val *
arr_proto_pop(js_val *instance, js_args *args, eval_state *state)
{
  unsigned long len = instance->object.length;
  if (len == 0) return JSUNDEF();

  js_val *key = JSNUMKEY(len-1);
  js_val *popped = fh_get(instance, key->string.ptr);

  fh_del_prop(instance, key->string.ptr);
  fh_set_len(instance, len - 1);
  return popped;
}

// Array.prototype.push(element1, ..., elementN)
js_val *
arr_proto_push(js_val *instance, js_args *args, eval_state *state)
{
  unsigned long len = instance->object.length;
  unsigned int nargs = ARGLEN(args);
  unsigned int i;
  for (i = 0; i < nargs; i++) {
    js_val *key = JSNUMKEY(len);
    fh_set(instance, key->string.ptr, ARG(args, i));
    len++;
  }

  fh_set_len(instance, len);
  return JSNUM(len);
}

// Array.prototype.reverse()
js_val *
arr_proto_reverse(js_val *instance, js_args *args, eval_state *state)
{
  unsigned long len = instance->object.length;
  unsigned long i = 0, j = len - 1;
  js_val *tmp;
  js_prop *iprop, *jprop;

  // While i & j converge, swap the values they point to.
  while (i < j) {
    char *ik = JSNUMKEY(i++)->string.ptr;
    char *jk = JSNUMKEY(j--)->string.ptr;

    iprop = fh_get_prop(instance, ik);
    jprop = fh_get_prop(instance, jk);

    // Need to do some checks to account for missing properties 
    // (they may have been deleted or never set)
    
    if (jprop)
      jprop->ptr = jprop->ptr == NULL ? JSUNDEF() : jprop->ptr;
    if (iprop)
      iprop->ptr = iprop->ptr == NULL ? JSUNDEF() : iprop->ptr;

    if (!iprop && jprop) {
      fh_set_prop(instance, ik, jprop->ptr, P_DEFAULT);
      fh_del_prop(instance, jk);
    }
    else if (!jprop && iprop) {
      fh_set_prop(instance, jk, iprop->ptr, P_DEFAULT);
      fh_del_prop(instance, ik);
    }
    else if (jprop && iprop) {
      tmp = iprop->ptr;
      iprop->ptr = jprop->ptr;
      jprop->ptr = tmp;
    }
  }

  return instance;
}

// Array.prototype.shift()
js_val *
arr_proto_shift(js_val *instance, js_args *args, eval_state *state)
{
  // Shift off first element, but then we have to rename all the keys.
  
  unsigned long len = instance->object.length;
  if (len == 0) 
    return JSUNDEF();

  char *key = "0";
  js_val *shifted = fh_get(instance, key);
  fh_del_prop(instance, key);

  unsigned long i;
  for (i = 1; i < len; i++) {
    js_val *old_key = JSNUMKEY(i);
    js_val *new_key = JSNUMKEY(i-1);
    js_prop *prop = fh_get_prop(instance, old_key->string.ptr);
    fh_del_prop(instance, old_key->string.ptr);
    fh_set(instance, new_key->string.ptr, prop->ptr);
  }

  fh_set_len(instance, len - 1);
  return shifted;
}

// Array.prototype.sort([compareFunction])
js_val *
arr_proto_sort(js_val *instance, js_args *args, eval_state *state)
{
  unsigned long len = instance->object.length;
  if (len == 0)
    return instance;

  js_val *maybe_cmp_func = ARG(args, 0);
  if (IS_FUNC(maybe_cmp_func)) {
    cmp_func = cmp_js;
    js_cmp_func = maybe_cmp_func;
    js_cmp_state = state;
  }
  else {
    cmp_func = cmp;
  }

  js_val *sorted = JSARR();

  // Build an array of js_prop pointers from the hashmap.
  unsigned long i, j;
  js_prop *prop, *prop_lst[len];
  for (i = 0; i < len; i++) {
    prop = fh_get_prop(instance, JSNUMKEY(i)->string.ptr);
    if (!prop) {
      i--;
      continue;
    }
    prop_lst[i] = prop;
  }

  // Do a merge sort of the js_prop pointers array.
  merge_sort(prop_lst, i);

  // Rebuild the hashmap (using a donor array).
  for (j = 0; j < i; j++) {
    prop = prop_lst[j];
    free(prop->name);
    prop->name = JSNUMKEY(j)->string.ptr;
    HASH_ADD_KEYPTR(hh, sorted->map, prop->name, strlen(prop->name), prop);
  }

  // Steal the donor array's hashmap.
  instance->map = sorted->map;
  sorted->map = NULL;

  fh_set_len(instance, len);

  return instance;
}

// Array.prototype.splice(index, howMany[, element1[, ..., elementN]])
js_val *
arr_proto_splice(js_val *instance, js_args *args, eval_state *state)
{
  js_val *index = ARG(args, 0);
  js_val *how_many = ARG(args, 1);

  js_val *rejects = JSARR();             // elements we've spliced out
  js_val *keepers = JSARR();             // elements we've kept + added elements

  unsigned long i = 0;                             // instance index
  unsigned long j = 0;                             // rejects index
  unsigned long k = 0;                             // keepers index
  unsigned long len = instance->object.length;     // instance array length
  unsigned long args_ind = 2;                      // args splice start index
  unsigned int args_len = ARGLEN(args);            // number of args
  unsigned long splice_ind = index->number.val;    // splice start index
  unsigned long splice_len = how_many->number.val; // splice length

  // For each element in the array.
  js_val *val;
  while (i < len) {

    // If we're at the splice point:
    if (i == splice_ind) {

      // Add any new elements
      while (args_ind < args_len) {
        fh_set(keepers, JSNUMKEY(k)->string.ptr, ARG(args, args_ind));
        args_ind++;
        k++;
      }
      // Add the spliced region to the rejects, skip over those indices.
      while (splice_len > 0) {
        val = fh_get(instance, JSNUMKEY(i)->string.ptr);
        fh_set(rejects, JSNUMKEY(j)->string.ptr, val);
        splice_len--, i++, j++;
      }
      // Don't hit this branch twice.
      splice_ind = -1;

    }
    else {
      val = fh_get(instance, JSNUMKEY(i)->string.ptr);
      fh_set(keepers, JSNUMKEY(k)->string.ptr, val); 
      k++, i++;
    }
  }

  // We're doing a hotswap of the keepers hash into the instance array.  
  // Still technically mutates the instance array (its pointer hasn't changed)
  instance->map = keepers->map;

  // GC will take the hash if we don't remove the reference.
  keepers->map = NULL;

  fh_set_len(instance, k);
  fh_set_len(rejects, j);
  return rejects;
}

// Array.prototype.unshift(element1, ..., elementN)
js_val *
arr_proto_unshift(js_val *instance, js_args *args, eval_state *state)
{
  js_val *newarr = JSARR();
  unsigned int nargs = ARGLEN(args);
  unsigned long len = instance->object.length;

  unsigned long i = 0, j = 0;

  // Add the args
  for (; i < nargs; i++) {
    fh_set(newarr, JSNUMKEY(i)->string.ptr, ARG(args, i));
  }

  // Add the instance's elements
  js_val *val;
  for (; j < len; j++, i++) {
    val = fh_get(instance, JSNUMKEY(j)->string.ptr);
    fh_set(newarr, JSNUMKEY(i)->string.ptr, val);
  }

  // Replace the map into our instance.
  instance->map = newarr->map;
  newarr->map = NULL;

  fh_set_len(instance, i);
  return JSNUM(i);
}

// Array.prototype.concat(value1, value2, ..., valueN)
js_val *
arr_proto_concat(js_val *instance, js_args *args, eval_state *state)
{
  unsigned int nargs = ARGLEN(args);
  unsigned long len = instance->object.length;
  js_val *concat = JSARR();
  js_val *key;

  unsigned long i = 0, // newarr index
                j = 0; // args index

  // Add the current array to the new array.
  for (; i < len; i++) {
    key = JSNUMKEY(i);
    fh_set(concat, key->string.ptr, fh_get(instance, key->string.ptr));
  }

  // Add the arguments to the new array.
  js_val *arg;
  for (; j < nargs; j++, i++) {
    key = JSNUMKEY(i);
    arg = ARG(args, j);
    // Extract array elements one level deep.
    if (IS_ARR(arg)) {
      unsigned long k;
      js_val *inner_key;
      for (k = 0; k < arg->object.length; k++) {
        key = JSNUMKEY(i);
        inner_key = JSNUMKEY(k);
        fh_set(concat, key->string.ptr, fh_get(arg, inner_key->string.ptr));
        // Let the outer loop increment i if we're at the end:
        if (k < arg->object.length - 1) i++;
      }
    } 
    else {
      fh_set(concat, key->string.ptr, arg);
    }
  }

  fh_set_len(concat, i);
  return concat;
}

static js_val *
do_join(js_val *arr, js_val *sep)
{
  js_val *result = JSSTR("");

  if (IS_UNDEF(sep)) sep = JSSTR(",");
  sep = TO_STR(sep);

  bool first = true;
  js_val *el;
  js_val *strval;
  unsigned long i;

  for (i = 0; i < arr->object.length; i++) {
    el = fh_get(arr, JSNUMKEY(i)->string.ptr);

    if (!first)
      result = JSSTR(fh_str_concat(result->string.ptr, sep->string.ptr));
    else
      first = false;

    strval = IS_UNDEF(el) || IS_NULL(el) ? JSSTR("") : TO_STR(el);
    result = JSSTR(fh_str_concat(result->string.ptr, strval->string.ptr));
  }

  return result;
}

// Array.prototype.join(separator)
js_val *
arr_proto_join(js_val *instance, js_args *args, eval_state *state)
{
  return do_join(instance, ARG(args, 0));
}

// Array.prototype.slice(begin[, end])
js_val *
arr_proto_slice(js_val *instance, js_args *args, eval_state *state)
{
  js_val *begin = ARG(args, 0);
  js_val *end = ARG(args, 1);
  js_val *slice = JSARR();
  unsigned long len = instance->object.length;

  unsigned long j;
  if (IS_UNDEF(begin) || begin->number.val > len)
    return slice;
  else if (begin->number.val >= 0)
    j = begin->number.val;
  else if (-begin->number.val <= len)
    j = len + begin->number.val;
  else
    j = 0;
  
  unsigned long k;
  if (IS_UNDEF(end) || end->number.val > len)
    k = len;
  else if (end->number.val >= 0)
    k = end->number.val;
  else if (-end->number.val <= len)
    k = len + end->number.val;
  else
    return slice;

  if (j >= k)
    return slice;

  // slice from j inclusive to k exclusive. 
  js_val *val;
  unsigned long i;
  for (i = 0; j < k && j < len; j++, i++) {
    val = fh_get(instance, JSNUMKEY(j)->string.ptr);
    fh_set(slice, JSNUMKEY(i)->string.ptr, val);
  }

  fh_set_len(slice, i);
  return slice;
}

// Array.prototype.toString()
js_val *
arr_proto_to_string(js_val *instance, js_args *args, eval_state *state)
{
  return do_join(instance, JSSTR(","));
}

// Array.prototype.indexOf(searchElement[, fromIndex])
js_val *
arr_proto_index_of(js_val *instance, js_args *args, eval_state *state)
{
  js_val *search = ARG(args, 0);
  js_val *from = ARG(args, 1);
  unsigned long len = instance->object.length;

  unsigned long i = 0;
  if (IS_NUM(from)) {
    if (from->number.val < 0)
      i = len + from->number.val;
    else
      i = from->number.val;
  }

  js_val *key, *equals;
  for (; i < len; i++) {
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
js_val *
arr_proto_last_index_of(js_val *instance, js_args *args, eval_state *state)
{
  js_val *search = ARG(args, 0);
  js_val *from = ARG(args, 1);
  unsigned long len = instance->object.length;
  long long i = len - 1;

  if (IS_NUM(from)) {
    if (from->number.val < 0)
      i = len + from->number.val;
    else if (from->number.val < len)
      i = from->number.val;
  }

  js_val *key, *equals;
  for (; i >= 0; i--) {
    key = JSNUMKEY(i);
    // lastIndexOf uses strict equality
    equals = fh_eq(fh_get(instance, key->string.ptr), search, true);
    if (equals->boolean.val) {
      return JSNUM(i);
    }
  }
  return JSNUM(-1);
}

// Array.prototype.filter(callback[, thisArg])
js_val *
arr_proto_filter(js_val *instance, js_args *args, eval_state *state)
{
  js_val *callback = ARG(args, 0);
  js_val *this = ARG(args, 1);
  js_val *filtered = JSARR();
  unsigned long len = instance->object.length;

  js_val *ikey, *jkey, *val, *result;
  js_args *cbargs;
  eval_state *call_state;
  unsigned long i = 0, j = 0;
  for (; i < len; i++) {
    ikey = JSNUMKEY(i);
    val = fh_get(instance, ikey->string.ptr);
    cbargs = fh_new_args(val, JSNUM(i), instance);
    call_state = fh_new_state(state->line, state->column);
    fh_push_state(call_state);
    result = fh_call(state->ctx, this, state, callback, cbargs);
    if (TO_BOOL(result)->boolean.val) {
      jkey = JSNUMKEY(j++);
      fh_set(filtered, jkey->string.ptr, fh_get(instance, ikey->string.ptr));
    }
  }

  fh_set_len(filtered, j);
  return filtered;
}

// Array.prototype.forEach(callback[, thisArg])
js_val *
arr_proto_for_each(js_val *instance, js_args *args, eval_state *state)
{
  js_val *callback = ARG(args, 0);
  js_val *this = ARG(args, 1);
  unsigned long len = instance->object.length;

  js_val *key, *val;
  js_args *cbargs;
  eval_state *call_state;
  unsigned long i;
  for (i = 0; i < len; i++) {
    key = JSNUMKEY(i);
    val = fh_get(instance, key->string.ptr);
    cbargs = fh_new_args(val, JSNUM(i), instance);
    call_state = fh_new_state(state->line, state->column);
    fh_push_state(call_state);
    fh_call(state->ctx, this, state, callback, cbargs);
  }

  return JSUNDEF();
}

// Array.prototype.every(callback[, thisArg])
js_val *
arr_proto_every(js_val *instance, js_args *args, eval_state *state)
{
  js_val *callback = ARG(args, 0);
  js_val *this = ARG(args, 1);
  unsigned long len = instance->object.length;

  js_val *key, *val, *result;
  js_args *cbargs;
  eval_state *call_state;
  unsigned long i;
  for (i = 0; i < len; i++) {
    key = JSNUMKEY(i);
    val = fh_get(instance, key->string.ptr);
    cbargs = fh_new_args(val, JSNUM(i), instance);
    call_state = fh_new_state(state->line, state->column);
    fh_push_state(call_state);
    result = fh_call(state->ctx, this, state, callback, cbargs);
    if (!TO_BOOL(result)->boolean.val)
      return JSBOOL(0);
  }

  return JSBOOL(1);
}

// Array.prototype.map(callback[, thisArg])
js_val *
arr_proto_map(js_val *instance, js_args *args, eval_state *state)
{
  js_val *callback = ARG(args, 0);
  js_val *this = ARG(args, 1);
  unsigned long len = instance->object.length;
  js_val *map = JSARR();

  js_val *key, *val, *result;
  js_args *cbargs;
  eval_state *call_state;
  unsigned long i;
  for (i = 0; i < len; i++) {
    key = JSNUMKEY(i);
    val = fh_get(instance, key->string.ptr);
    cbargs = fh_new_args(val, JSNUM(i), instance);
    call_state = fh_new_state(state->line, state->column);
    fh_push_state(call_state);
    result = fh_call(state->ctx, this, state, callback, cbargs);
    fh_set(map, key->string.ptr, result);
  }

  fh_set_len(map, len);
  return map;
}

// Array.prototype.some(callback[, thisArg])
js_val *
arr_proto_some(js_val *instance, js_args *args, eval_state *state)
{
  js_val *callback = ARG(args, 0);
  js_val *this = ARG(args, 1);
  unsigned long len = instance->object.length;

  js_val *key, *val, *result;
  js_args *cbargs;
  eval_state *call_state;
  unsigned long i;
  for (i = 0; i < len; i++) {
    key = JSNUMKEY(i);
    val = fh_get(instance, key->string.ptr);
    cbargs = fh_new_args(val, JSNUM(i), instance);
    call_state = fh_new_state(state->line, state->column);
    fh_push_state(call_state);
    result = fh_call(state->ctx, this, state, callback, cbargs);
    if (TO_BOOL(result)->boolean.val)
      return JSBOOL(1);
  }

  return JSBOOL(0);
}

// Array.prototype.reduce(callback[, seed])
js_val *
arr_proto_reduce(js_val *instance, js_args *args, eval_state *state)
{
  js_val *callback = ARG(args, 0);
  if (!IS_FUNC(callback))
    fh_error(state, E_TYPE, "%s is not a function", fh_typeof(callback));

  js_val *reduction = ARG(args, 1);
  unsigned long i = 0, len = instance->object.length;

  if (IS_UNDEF(reduction)) {
    if (len == 0)
      fh_error(state, E_RANGE, "Reduce of empty array with no initial value");

    reduction = fh_get(instance, "0");
    i = 1;
  }

  js_val *key, *val;
  js_args *cbargs;
  eval_state *call_state;
  for (; i < len; i++) {
    key = JSNUMKEY(i);
    val = fh_get(instance, key->string.ptr);
    cbargs = fh_new_args(reduction, val, JSNUM(i));
    call_state = fh_new_state(state->line, state->column);
    fh_push_state(call_state);
    reduction = fh_call(state->ctx, JSUNDEF(), state, callback, cbargs);
  }

  return reduction;
}

// Array.prototype.reduceRight(callback[, seed])
js_val *
arr_proto_reduce_right(js_val *instance, js_args *args, eval_state *state)
{
  js_val *callback = ARG(args, 0);
  if (!IS_FUNC(callback))
    fh_error(state, E_TYPE, "%s is not a function", fh_typeof(callback));

  js_val *reduction = ARG(args, 1);
  unsigned long len = instance->object.length;
  unsigned long i = len - 1;

  if (IS_UNDEF(reduction)) {
    if (len == 0)
      fh_error(state, E_RANGE, "Reduce of empty array with no initial value");

    reduction = fh_get(instance, JSNUMKEY(i)->string.ptr);

    if (len == 1)
      return reduction;
    
    i--;
  }
  else {
    if (len == 0)
      return reduction;
  }

  js_val *val;
  js_args *cbargs;
  eval_state *call_state;
  do {
    val = fh_get(instance, JSNUMKEY(i)->string.ptr);
    cbargs = fh_new_args(reduction, val, JSNUM(i));
    call_state = fh_new_state(state->line, state->column);
    fh_push_state(call_state);
    reduction = fh_call(state->ctx, JSUNDEF(), state, callback, cbargs);
  } while (i--);

  return reduction;
}

js_val *
bootstrap_array()
{
  js_val *array = JSNFUNC(arr_new, 1);
  js_val *proto = JSARR();
  proto->proto = fh->object_proto;

  // Array
  // -----

  // Properties
  DEF(array, "prototype", proto);

  // Methods
  DEF(array, "isArray", JSNFUNC(arr_is_array, 1));

  // Array.prototype
  // ---------------

  // Properties
  DEF(proto, "constructor", JSNFUNC(arr_new, 1));
  DEF(proto, "length", JSNUM(0));

  // Methods
  DEF(proto, "pop", JSNFUNC(arr_proto_pop, 0));
  DEF(proto, "push", JSNFUNC(arr_proto_push, 1));
  DEF(proto, "reverse", JSNFUNC(arr_proto_reverse, 0));
  DEF(proto, "shift", JSNFUNC(arr_proto_shift, 0));
  DEF(proto, "sort", JSNFUNC(arr_proto_sort, 1));
  DEF(proto, "splice", JSNFUNC(arr_proto_splice, 2));
  DEF(proto, "unshift", JSNFUNC(arr_proto_unshift, 1));

  DEF(proto, "concat", JSNFUNC(arr_proto_concat, 1));
  DEF(proto, "join", JSNFUNC(arr_proto_join, 1));
  DEF(proto, "slice", JSNFUNC(arr_proto_slice, 2));
  DEF(proto, "toString", JSNFUNC(arr_proto_to_string, 0));
  DEF(proto, "indexOf", JSNFUNC(arr_proto_index_of, 1));
  DEF(proto, "lastIndexOf", JSNFUNC(arr_proto_last_index_of, 1));

  DEF(proto, "filter", JSNFUNC(arr_proto_filter, 1));
  DEF(proto, "forEach", JSNFUNC(arr_proto_for_each, 1));
  DEF(proto, "every", JSNFUNC(arr_proto_every, 1));
  DEF(proto, "map", JSNFUNC(arr_proto_map, 1));
  DEF(proto, "some", JSNFUNC(arr_proto_some, 1));
  DEF(proto, "reduce", JSNFUNC(arr_proto_reduce, 1));
  DEF(proto, "reduceRight", JSNFUNC(arr_proto_reduce_right, 1));

  fh_attach_prototype(proto, fh->function_proto);
  fh->array_proto = proto;

  return array;
}
