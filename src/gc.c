/*
 * gc.c -- slotted memory management and mark-sweep garbage collection
 *
 * Copyright (c) 2012 Nick Reynolds
 *  
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stddef.h>
#include "gc.h"


PoolMetadata *pool;

JSValue *
fh_malloc(bool first_attempt)
{
  PoolMetadata *pool = fh_get_pool();
  int i;
  for (i = 0; i < SLOTS_PER_POOL; i++) {
    if (!pool->freelist[i]) {
      pool->freelist[i] = true;
      return &(pool->slots[i]);
    }
  }
  if (first_attempt) {
    fh_gc();
    return fh_malloc(false);
  } 
  fh_error(NULL, E_ERROR, "process out of memory");
  UNREACHABLE();
}

PoolMetadata *
fh_new_pool()
{
  size_t pool_size = sizeof(JSValue) * SLOTS_PER_POOL;
  JSValue *slots = malloc(pool_size);
  PoolMetadata *pool = malloc(sizeof(PoolMetadata));
  pool->num_slots = SLOTS_PER_POOL;
  pool->slots = slots;
  pool->global = NULL;
  int i;
  for (i = 0; i < SLOTS_PER_POOL; i++)
    pool->freelist[i] = false;
  return pool;
}

PoolMetadata *
fh_get_pool()
{
  if (!pool) 
    pool = fh_new_pool();
  return pool;
}

JSValue *
fh_global()
{
  return fh_get_pool()->global;
}

void
fh_gc()
{
#ifdef fh_gc_profile
  puts("GC running.");
#endif
  PoolMetadata *pool = fh_get_pool();
  fh_gc_mark(pool->global);
  fh_gc_sweep(pool);
}

void
fh_gc_register_global(JSValue *global)
{
  PoolMetadata *pool = fh_get_pool();
  pool->global = global;
}

void
fh_gc_mark(JSValue *val)
{
  if (!val || val->marked) return;

  val->marked = true;

  if (val->proto) fh_gc_mark(val->proto);

  if (val->type == T_FUNCTION) {
    fh_gc_mark(val->function.closure);
    fh_gc_mark(val->function.bound_this);
    fh_gc_mark(val->function.instance);
  }

  if (val->type == T_OBJECT) {
    JSProp *prop;
    OBJ_ITER(val, prop) {
      if (prop->ptr && !prop->circular) 
        fh_gc_mark(prop->ptr);
    }
  }
}

void
fh_gc_sweep(PoolMetadata *pool)
{
  int i;
  JSValue val;
  for (i = 0; i < SLOTS_PER_POOL; i++) {
    val = pool->slots[i];
    if (!val.marked) {
      pool->freelist[i] = false;
      fh_gc_free_val(&val);
    }
    val.marked = false;
  }
}

void
fh_gc_free_val(JSValue *val)
{
  /*
  // Free the object hashtable 
  // TODO: just objects?
  if (val->type == T_OBJECT) {
    JSProp *prop, *tmp;
    HASH_ITER(hh, val->map, prop, tmp) {
      HASH_DEL(val->map, prop);
      if (prop != NULL) free(prop);
    }
  }
  */
  // Free any strings (dynamically alloc-ed outside slots)
  if (IS_STR(val) && val->string.ptr != NULL) {
    free(val->string.ptr);
  }

  memset(val, 0, sizeof(JSValue));
}
