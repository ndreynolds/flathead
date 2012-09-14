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

#include "gc.h"

ContainerMetadata *container;

JSValue *
fh_malloc(bool first_attempt)
{
  ContainerMetadata *container = fh_get_container();
  int i;
  for (i=0; i<SLOTS_PER_CONTAINER; i++) {
    if (!container->freelist[i]) {
      container->freelist[i] = true;
      return &(container->slots[i]);
    }
  }
  if (first_attempt) {
    fh_gc();
    return fh_malloc(false);
  } 
  fh_error(NULL, E_ERROR, "process out of memory");
}

ContainerMetadata *
fh_new_container()
{
  size_t container_size = sizeof(JSValue) * SLOTS_PER_CONTAINER;
  JSValue *slots = malloc(container_size);
  ContainerMetadata *container = malloc(sizeof(ContainerMetadata));
  container->num_slots = SLOTS_PER_CONTAINER;
  container->slots = slots;
  return container;
}

ContainerMetadata *
fh_get_container()
{
  if (!container) 
    container = fh_new_container();
  return container;
}

void
fh_gc()
{
  ContainerMetadata *container = fh_get_container();
  fh_gc_mark(container->global);
  fh_gc_sweep(container);
}

void
fh_gc_register_global(JSValue *global)
{
  ContainerMetadata *container = fh_get_container();
  container->global = global;
}

void
fh_gc_mark(JSValue *obj)
{
  obj->marked = true;
  JSProp *prop, *tmp;
  JSValue *val;
  HASH_ITER(hh, obj->object.map, prop, tmp) {
    val = prop->ptr;
    if (val == NULL) continue;
    if (val->type == T_OBJECT && !prop->circular) 
      fh_gc_mark(val);
    else
      val->marked = true;
  }
}

void
fh_gc_sweep(ContainerMetadata *container)
{
  int i;
  JSValue val;
  for (i=0; i<SLOTS_PER_CONTAINER; i++) {
    val = container->slots[i];
    if (!val.marked) {
      container->freelist[i] = false;
      fh_gc_free_val(&val);
    }
  }
}

void
fh_gc_free_val(JSValue *val)
{
  /*
  printf("Freeing %s (%d)\n", fh_typeof(val), val->type);
  // Free the object hashtable
  if (val->type == T_OBJECT) {
    JSProp *prop, *tmp;
    HASH_ITER(hh, val->object.map, prop, tmp) {
      HASH_DEL(val->object.map, prop);
      if (prop != NULL) free(prop);
    }
  }
  // Free any strings (dynamically alloc-ed outside slots)
  if (val->string.ptr != NULL) {
    printf("(String '%s')\n", val->string.ptr);
    free(val->string.ptr);
  }
  */

  memset(val, 0, sizeof(JSValue));
}
