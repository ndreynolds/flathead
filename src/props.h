/*
 * props.h -- Property access and prototype/scope traversals
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

void fh_set(js_val *, char *, js_val *);
void fh_set_prop(js_val *, char *, js_val *, js_prop_flags);
void fh_set_rec(js_val *, char *, js_val *);
bool fh_del_prop(js_val *, char *);
js_prop * fh_get_prop(js_val *, char *);
js_prop * fh_get_prop_rec(js_val *, char *);
js_prop * fh_get_prop_proto(js_val *, char *);
js_val * fh_get(js_val *, char *);
js_val * fh_get_proto(js_val *, char *);
js_val * fh_get_rec(js_val *, char *);

#endif
