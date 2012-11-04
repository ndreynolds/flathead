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


gc_arena *arena;

js_val *
fh_malloc(bool first_attempt)
{
  gc_arena *arena = fh_get_arena();
  int i;
  for (i = 0; i < SLOTS_PER_ARENA; i++) {
    if (!arena->freelist[i]) {
      arena->freelist[i] = true;
      return &(arena->slots[i]);
    }
  }
  if (first_attempt) {
    fh_gc();
    return fh_malloc(false);
  } 
  fh_error(NULL, E_ERROR, "process out of memory");
  UNREACHABLE();
}

gc_arena *
fh_new_arena()
{
  size_t arena_size = sizeof(js_val) * SLOTS_PER_ARENA;
  js_val *slots = malloc(arena_size);
  gc_arena *arena = malloc(sizeof(gc_arena));
  arena->num_slots = SLOTS_PER_ARENA;
  arena->slots = slots;
  arena->global = NULL;
  int i;
  for (i = 0; i < SLOTS_PER_ARENA; i++)
    arena->freelist[i] = false;
  return arena;
}

gc_arena *
fh_get_arena()
{
  if (!arena) 
    arena = fh_new_arena();
  return arena;
}

js_val *
fh_global()
{
  return fh_get_arena()->global;
}

void
fh_gc()
{
#ifdef fh_gc_profile
  puts("GC running.");
#endif
  gc_arena *arena = fh_get_arena();
  fh_gc_mark(arena->global);
  fh_gc_sweep(arena);
}

void
fh_gc_register_global(js_val *global)
{
  gc_arena *arena = fh_get_arena();
  arena->global = global;
}

void
fh_gc_mark(js_val *val)
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
    js_prop *prop;
    OBJ_ITER(val, prop) {
      if (prop->ptr && !prop->circular) 
        fh_gc_mark(prop->ptr);
    }
  }
}

void
fh_gc_sweep(gc_arena *arena)
{
  int i;
  js_val val;
  for (i = 0; i < SLOTS_PER_ARENA; i++) {
    val = arena->slots[i];
    if (!val.marked) {
      arena->freelist[i] = false;
      fh_gc_free_val(&val);
    }
    val.marked = false;
  }
}

void
fh_gc_free_val(js_val *val)
{
  /*
  // Free the object hashtable 
  // TODO: just objects?
  if (val->type == T_OBJECT) {
    js_prop *prop, *tmp;
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

  memset(val, 0, sizeof(js_val));
}
