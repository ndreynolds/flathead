/*
 * eval.h -- AST-walker
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

#ifndef EVAL_H
#define EVAL_H

#include "flathead.h"
#include "nodes.h"

#define T_BOTH(a,b,t)     ((a)->type == (t) && (b)->type == (t))
#define T_XOR(a,b,t1,t2)  (((a)->type == (t1) && (b)->type == (t2)) || \
                           ((a)->type == (t2) && (b)->type == (t1)))

js_val * fh_eval(js_val *, ast_node *);
js_val * fh_call(js_val *, js_val *, eval_state *, js_val *, js_args *);
js_val * fh_eq(js_val *, js_val *, bool);

#endif
