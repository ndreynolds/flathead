/*
 * props.h -- Property access and hashmap traversals
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

#ifndef PROP_H
#define PROP_H

#include "flathead.h"

void fh_set(JSValue *, char *, JSValue *);
void fh_set_prop(JSValue *, char *, JSValue *, JSPropFlags);
void fh_set_rec(JSValue *, char *, JSValue *);
void fh_del_prop(JSValue *, char *);
JSProp * fh_get_prop(JSValue *, char *);
JSProp * fh_get_prop_rec(JSValue *, char *);
JSProp * fh_get_prop_proto(JSValue *, char *);
JSValue * fh_get(JSValue *, char *);
JSValue * fh_get_proto(JSValue *, char *);
JSValue * fh_get_rec(JSValue *, char *);

#endif
