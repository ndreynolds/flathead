/*
 * gc.h -- slotted memory management and mark-sweep garbage collection
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

#ifndef GC_H
#define GC_H

#include "flathead.h"

#define SLOTS_PER_ARENA 10000

typedef struct {
  js_val *slots;
  js_val *global;
  int num_slots;
  bool freelist[SLOTS_PER_ARENA];
} gc_arena;

gc_arena * fh_new_arena(void);
gc_arena * fh_get_arena(void);
js_val * fh_global(void);
js_val * fh_malloc(bool);
void fh_gc(void);
void fh_gc_register_global(js_val *);
void fh_gc_mark(js_val *);
void fh_gc_free_val(js_val *);
void fh_gc_sweep(gc_arena *);

#endif
