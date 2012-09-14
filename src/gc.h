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

#include <stddef.h>
#include "flathead.h"

#define SLOTS_PER_CONTAINER 1000

typedef struct ContainerMetadata {
  JSValue *slots;
  JSValue *global;
  int num_slots;
  bool freelist[SLOTS_PER_CONTAINER];
} ContainerMetadata;

ContainerMetadata * fh_new_container(void);
ContainerMetadata * fh_get_container(void);
JSValue * fh_malloc(bool);
void fh_gc(void);
void fh_gc_register_global(JSValue *);
void fh_gc_mark(JSValue *);
void fh_gc_free_val(JSValue *);
void fh_gc_sweep(ContainerMetadata *);

#endif
