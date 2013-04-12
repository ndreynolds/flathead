/*
 * runtime.h -- Bootstrap global object and friends 
 * 
 * Copyright (c) 2012-2013 Nick Reynolds
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

#ifndef RUNTIME_H
#define RUNTIME_H

#include "../flathead.h"
#include "../props.h"
#include "../str.h"
#include "../regexp.h"
#include "../debug.h"
#include "../eval.h"
#include "../gc.h"

js_val * global_is_nan(js_val *, js_args *, eval_state *);
js_val * global_is_finite(js_val *, js_args *, eval_state *);
js_val * global_parse_float(js_val *, js_args *, eval_state *);
js_val * glboal_parse_int(js_val *, js_args *, eval_state *);
js_val * global_eval(js_val *, js_args *, eval_state *);
js_val * global_gc(js_val *, js_args *, eval_state *);

void fh_attach_prototype(js_val *, js_val *);
js_val * fh_bootstrap(void);

#endif
