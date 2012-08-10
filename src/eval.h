/*
 * eval.h -- AST-walker
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

#include "nodes.h"

#define T_BOTH(a,b,t)      (a->type == t && b->type == t)
#define T_XOR(a,b,t1,t2)   (a->type == t1 && b->type == t2 || a->type == t2 && b->type == t1)

void jl_while(JLVALUE *, JLNode *, JLNode *);
JLARGS * jl_build_args(JLVALUE *, JLNode *);
JLVALUE * jl_eval(JLVALUE *, JLNode *);
JLVALUE * jl_obj(JLNode *);
JLVALUE * jl_get(JLVALUE *, char *);
JLVALUE * jl_call(JLVALUE *, JLNode *);
JLVALUE * jl_function_call(JLVALUE *, JLVALUE *, JLNode *);
JLVALUE * jl_member(JLVALUE *, JLNode *);
JLVALUE * jl_eval_exp(JLVALUE *, JLNode *);
JLVALUE * jl_eval_postfix_exp(JLVALUE *, JLNode *);
JLVALUE * jl_eval_prefix_exp(JLVALUE *, JLNode *);
JLVALUE * jl_eval_bin_exp(JLVALUE *, JLNode *);
JLVALUE * jl_add(JLVALUE *, JLVALUE *);
JLVALUE * jl_sub(JLVALUE *, JLVALUE *);
JLVALUE * jl_mul(JLVALUE *, JLVALUE *);
JLVALUE * jl_div(JLVALUE *, JLVALUE *);
JLVALUE * jl_mod(JLVALUE *, JLVALUE *);
JLVALUE * jl_eq(JLVALUE *, JLVALUE *, bool);
JLVALUE * jl_neq(JLVALUE *, JLVALUE *, bool);
JLVALUE * jl_gt(JLVALUE *, JLVALUE *);
JLVALUE * jl_lt(JLVALUE *, JLVALUE *);
JLVALUE * jl_and(JLVALUE *, JLVALUE *);
JLVALUE * jl_or(JLVALUE *, JLVALUE *);
