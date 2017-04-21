/*
 * gc.c -- slotted memory management and mark-sweep garbage collection
 *
 * Copyright (c) 2012-2017 Nick Reynolds
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

#ifdef FH_GC_PROFILE
#define GC_PRINT(indent, ...) printf("%*s", indent, ""); printf(__VA_ARGS__)
#define GC_DEBUG(indent, val) fh_debug(stdout, val, indent, true)
#else
#define GC_PRINT(indent, ...)
#define GC_DEBUG(indent, val)
#endif

#ifdef FH_GC_PROFILE_VERBOSE
#define GC_PRINT_VERBOSE(indent, ...) GC_PRINT(indent, ...)
#define GC_DEBUG_VERBOSE(indent, val) GC_DEBUG(indent, val)
#else
#define GC_PRINT_VERBOSE(indent, ...)
#define GC_DEBUG_VERBOSE(indent, val)
#endif

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


static gc_arena *
fh_new_arena()
{
  gc_arena *arena = malloc(sizeof(gc_arena));

  arena->num_slots = SLOTS_PER_ARENA;
  arena->used_slots = 0;

  memset(arena->freelist, 0, sizeof(arena->freelist));
  return arena;
}

static gc_arena *
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

js_val *
fh_malloc(bool first_attempt)
{
  if (fh->gc_state != GC_STATE_NONE) {
    fprintf(stderr, "Error: politely refusing to allocate during garbage collection");
    exit(EXIT_FAILURE);
  }

  gc_arena *arena = fh_get_arena();
  int i;
  for (i = 0; i < arena->num_slots; i++) {
    if (!arena->freelist[i]) {
      arena->freelist[i] = true;
      arena->used_slots++;
      return &arena->slots[i];
    }
  }
  if (first_attempt) {
    fh_gc();
    return fh_malloc(false);
  }
  fprintf(stderr, "Error: process out of memory");
  exit(EXIT_FAILURE);
  UNREACHABLE();
}

static void
fh_gc_debug()
{
#ifdef FH_GC_PROFILE

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
    "  %ld slots | %ld/%ld used slots | %ld/%ld vacant slots | %ld usage KB | %d runs\n",
    total_slots,
    used_slots, total_slots,
    total_slots - used_slots, total_slots,
    (total_slots * sizeof(js_val)) / 1000,
    fh->gc_runs
  );

#endif
}

static void
fh_gc_debug_arena(gc_arena *arena)
{
#ifdef FH_GC_PROFILE_VERBOSE
  for (int i = 0; i < arena->num_slots; i++) {
    printf("slot[%4d]: (USED %d) (MARKED %d)\n", i, arena->freelist[i], arena->freelist[i] && arena->slots[i].marked);
  }
#endif
}

static void
fh_gc_mark(js_val *val, int depth)
{
  if (val && val->flagged) puts("Attempting to mark flagged val");
  if (!val || val->marked) return;

  val->marked = true;

  GC_DEBUG_VERBOSE(depth, val);

  GC_PRINT_VERBOSE(depth, "Marking prototype\n");
  fh_gc_mark(val->proto, depth + 1);

  if (IS_OBJ(val)) {
    GC_PRINT_VERBOSE(depth, "Marking primitive\n");
    fh_gc_mark(val->object.primitive, depth + 1);

    GC_PRINT_VERBOSE(depth, "Marking bound this\n");
    fh_gc_mark(val->object.bound_this, depth + 1);

    GC_PRINT_VERBOSE(depth, "Marking scope\n");
    fh_gc_mark(val->object.scope, depth + 1);

    GC_PRINT_VERBOSE(depth, "Marking instance\n");
    fh_gc_mark(val->object.instance, depth + 1);

    GC_PRINT_VERBOSE(depth, "Marking parent\n");
    fh_gc_mark(val->object.parent, depth + 1);
  }

  if (val->map) {
    js_prop *prop;
    OBJ_ITER(val, prop) {
      if (prop->ptr && !prop->circular) {
        GC_PRINT_VERBOSE(depth, "Marking %s\n", prop->name);
        fh_gc_mark(prop->ptr, depth + 1);
      }
    }
  }
}

static void
fh_gc_free_val(js_val *val)
{
  // Free the object hashtable
  //
  // Note we're not freeing the values pointed at, only the pointers to them
  // and the hashtable overhead.
  /* if (val->map) { */
  /*   js_prop *prop, *tmp; */
  /*   HASH_ITER(hh, val->map, prop, tmp) { */
  /*     HASH_DEL(val->map, prop); */
  /*     if (prop != NULL) { */
  /*       free(prop); */
  /*     } */
  /*   } */
  /* } */

  // Free any strings (dynamically alloc-ed outside slots)
  if (IS_STR(val) && val->string.ptr != NULL) {
    free(val->string.ptr);
  }

  memset(val, 0, sizeof(js_val));
}

static void
fh_gc_sweep(gc_arena *arena)
{
  js_val val;
  int sweeped_count = 0;
  for (int i = 0; i < arena->num_slots; i++) {
    if (!arena->freelist[i]) return;
    val = arena->slots[i];
    if (!val.marked) {
      if (val.flagged) puts("GC: freeing flagged val");
      arena->freelist[i] = false;
      fh_gc_free_val(&val);
      arena->used_slots--;
      sweeped_count++;
    } else {
      arena->slots[i].marked = false;
    }
  }
}

void
fh_gc()
{
  gc_arena *arena = fh_get_arena();

  fh_gc_debug_arena(arena);

  // Start
  fh->gc_state = GC_STATE_STARTING;
  fh_gc_debug();

  // Mark
  fh->gc_state = GC_STATE_MARK;
  fh_gc_mark(fh->global, 0);
  if (fh->callstack) {
    eval_state *top = fh->callstack;
    while (top) {
      fh_gc_mark(top->scope, 0);
      top = top->parent;
    }
  }
  fh_gc_debug();

  // Sweep
  fh->gc_state = GC_STATE_SWEEP;
  fh_gc_sweep(arena);
  fh_gc_debug();

  // Stop
  fh->gc_state = GC_STATE_NONE;
  fh_gc_debug();

  fh_gc_debug_arena(arena);
}
