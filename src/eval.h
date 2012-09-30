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

#ifndef EVAL_H
#define EVAL_H

#include "nodes.h"
#include "props.h"

#define T_BOTH(a,b,t)      (a->type == t && b->type == t)
#define T_XOR(a,b,t1,t2)   ((a->type == t1 && b->type == t2) || (a->type == t2 && b->type == t1))

void fh_while(JSValue *, Node *, Node *);
void fh_for(JSValue *, Node *, Node *);
void fh_forin(JSValue *, Node *);
void fh_do_assign(JSValue *, char *, JSValue *, char *);
JSArgs * fh_build_args(JSValue *, Node *);
JSValue * fh_break(void);
JSValue * fh_eval(JSValue *, Node *);
JSValue * fh_eval_each(JSValue *, Node *);
JSValue * fh_exp(JSValue *, Node *);
JSValue * fh_if(JSValue *, Node *);
JSValue * fh_assign(JSValue *, Node *);
JSValue * fh_return(JSValue *, Node *);
JSValue * fh_prop_lst(JSValue *, Node *);
JSValue * fh_stmt_lst(JSValue *, Node *);
JSValue * fh_src_lst(JSValue *, Node *);
JSValue * fh_obj(JSValue *, Node *);
JSValue * fh_arr(JSValue *, Node *);
JSValue * fh_call(JSValue *, Node *);
JSValue * fh_var_dec(JSValue *, Node *);
JSValue * fh_function_call(JSValue *, State *, JSValue *, JSArgs *);
JSValue * fh_setup_func_env(JSValue *, JSValue *, JSArgs *);
JSValue * fh_str_from_node(JSValue *, Node *);
JSValue * fh_member(JSValue *, Node *);
JSValue * fh_eval_postfix_exp(JSValue *, Node *);
JSValue * fh_eval_prefix_exp(JSValue *, Node *);
JSValue * fh_eval_bin_exp(JSValue *, Node *);
JSValue * fh_add(JSValue *, JSValue *);
JSValue * fh_sub(JSValue *, JSValue *);
JSValue * fh_mul(JSValue *, JSValue *);
JSValue * fh_div(JSValue *, JSValue *);
JSValue * fh_mod(JSValue *, JSValue *);
JSValue * fh_eq(JSValue *, JSValue *, bool);
JSValue * fh_neq(JSValue *, JSValue *, bool);
JSValue * fh_gt(JSValue *, JSValue *);
JSValue * fh_lt(JSValue *, JSValue *);
JSValue * fh_and(JSValue *, Node *, Node *);
JSValue * fh_or(JSValue *, Node *, Node *);

#endif
