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
#include <time.h>

#include "gc.h"
#include "debug.h"

/* GC Overview
 *
 * Bi-color, non-incremental, mark & sweep, stop-the-world garbage collection.
 *
 * Arenas
 * ------
 * The global state object keeps an array of pointers to arenas – big
 * contiguous blocks of memory which are slotted to hold Flathead's js_val
 * structs. The arena size is determined by the SLOTS_PER_ARENA define. The
 * size is then that number of slots multiplied by the size of a js_val struct.
 * The system will create arenas on demand, when space cannot be freed from an
 * existing arena, and the MAX_ARENAS define has not been exceeded.
 *
 * At the start of each arena is a metadata section which stores usage
 * information and the list of vacant slots.
 *
 * Allocation
 * ----------
 *
 * Mark Phase
 * ----------
 *
 * Sweep Phase
 * -----------
 *
 * Issues & Enhancement Ideas
 * --------------------------
 * - Arena free lists should be bitmaps. Integer arrays are a huge waste of
 *   space.
 * - Store the color (e.g. black or white) of the js_val instead of explicitly
 *   labeling them marked or unmarked. Then we can flip the color semantics
 *   after each run and save some time unmarking.
 * - Tri- or quad-color incremental algorithm a la LuaJIT.
 * - Utility structs (e.g. js_args, js_prop, eval_state, ast_nodes) need to be
 *   garbage collected or freed by hand, whichever is more appropriate.
 * - Strings need to be stored within the arena somehow. Maybe they can be
 *   broken into js_val-sized pieces and be read and written to through an API
 *   that handles any needed reassembly.
 */


js_val *
fh_malloc(bool first_attempt)
{
  gc_arena *arena = fh_get_arena();
  int i;
  for (i = 0; i < arena->num_slots; i++) {
    if (!arena->freelist[i]) {
      arena->freelist[i] = true;
      arena->used_slots++;
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
  arena->used_slots = 0;
  arena->slots = slots;

  memset(arena->freelist, 0, sizeof(arena->freelist));
  return arena;
}

gc_arena *
fh_get_arena()
{
  // Currently 1 arena, but room for more.
  gc_arena *arena = NULL;
  if (fh->gc_num_arenas == 0) {
    arena = fh_new_arena();
    fh->gc_arenas[0] = arena;
    fh->gc_num_arenas++;
  }
  else
    arena = fh->gc_arenas[0];
  return arena;
}

void
fh_gc_debug()
{
#ifdef fh_gc_profile

  gc_state state = fh->gc_state;
  long clockt = clock() / (CLOCKS_PER_SEC / 1000);

  if (state == GC_STATE_STARTING) {
    puts("GC: starting");
    fh->gc_last_start = clockt;
    fh->gc_runs++;
  }
  if (state == GC_STATE_MARK) {
    puts("GC: mark phase");
  }
  if (state == GC_STATE_SWEEP) {
    puts("GC: sweep phase");
  }
  if (state == GC_STATE_NONE) {
    long delta = clockt - fh->gc_last_start;
    printf("GC: finished|inactive (last run: %ld ms)\n", delta);
    fh->gc_time += delta;
  }

  long total_slots = 0, used_slots = 0;
  int i;
  for (i = 0; i < fh->gc_num_arenas; i++) {
    total_slots += fh->gc_arenas[i]->num_slots;
    used_slots += fh->gc_arenas[i]->used_slots;
  }

  printf(
    "  %ld slots | %ld/%ld used slots | %ld/%ld vacant slots | %ld usage (bytes) | %d runs\n",
    total_slots,
    used_slots, total_slots,
    total_slots - used_slots, total_slots,
    total_slots * sizeof(js_val),
    fh->gc_runs
  );

#endif
}

void
fh_gc()
{
  gc_arena *arena = fh_get_arena();

  // Start
  fh->gc_state = GC_STATE_STARTING;
  fh_gc_debug();

  // Mark
  fh->gc_state = GC_STATE_MARK;
  fh_gc_mark(fh->global);
  fh_gc_debug();

  // Sweep
  fh->gc_state = GC_STATE_SWEEP;
  fh_gc_sweep(arena);
  fh_gc_debug();

  // Stop
  fh->gc_state = GC_STATE_NONE;
  fh_gc_debug();
}

void
fh_gc_mark(js_val *val)
{
  if (!val || val->marked) return;

  val->marked = true;

  fh_gc_mark(val->proto);

  if (IS_OBJ(val)) {
    fh_gc_mark(val->object.primitive);
    fh_gc_mark(val->object.bound_this);
    fh_gc_mark(val->object.scope);
    fh_gc_mark(val->object.instance);
    fh_gc_mark(val->object.parent);
  }

  if (val->map) {
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
      arena->used_slots--;
    }
    val.marked = false;
  }
}

void
fh_gc_free_val(js_val *val)
{
  // Free the object hashtable 
  //
  // Note we're not freeing the values pointed at, only the pointers to them
  // and the hashtable overhead.
  /*
  if (val->map) {
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
