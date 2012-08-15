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

void fh_while(JSVALUE *, JSNODE *, JSNODE *);
void fh_assign(JSVALUE *, char *, JSVALUE *, char *);
JSARGS * fh_build_args(JSVALUE *, JSNODE *);
JSVALUE * fh_eval(JSVALUE *, JSNODE *);
JSVALUE * fh_stmt_lst(JSVALUE *, JSNODE *);
JSVALUE * fh_obj(JSVALUE *, JSNODE *);
JSVALUE * fh_arr(JSVALUE *, JSNODE *);
JSVALUE * fh_call(JSVALUE *, JSNODE *);
JSVALUE * fh_function_call(JSVALUE *, JSVALUE *, JSNODE *);
JSVALUE * fh_setup_func_env(JSVALUE *, JSVALUE *, JSARGS *);
JSVALUE * fh_member(JSVALUE *, JSNODE *);
JSVALUE * fh_eval_postfix_exp(JSVALUE *, JSNODE *);
JSVALUE * fh_eval_prefix_exp(JSVALUE *, JSNODE *);
JSVALUE * fh_eval_bin_exp(JSVALUE *, JSNODE *);
JSVALUE * fh_add(JSVALUE *, JSVALUE *);
JSVALUE * fh_sub(JSVALUE *, JSVALUE *);
JSVALUE * fh_mul(JSVALUE *, JSVALUE *);
JSVALUE * fh_div(JSVALUE *, JSVALUE *);
JSVALUE * fh_mod(JSVALUE *, JSVALUE *);
JSVALUE * fh_eq(JSVALUE *, JSVALUE *, bool);
JSVALUE * fh_neq(JSVALUE *, JSVALUE *, bool);
JSVALUE * fh_gt(JSVALUE *, JSVALUE *);
JSVALUE * fh_lt(JSVALUE *, JSVALUE *);
JSVALUE * fh_and(JSVALUE *, JSNODE *, JSNODE *);
JSVALUE * fh_or(JSVALUE *, JSNODE *, JSNODE *);
