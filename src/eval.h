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

#include "flathead.h"
#include "nodes.h"

#define T_BOTH(a,b,t)      (a->type == t && b->type == t)
#define T_XOR(a,b,t1,t2)   ((a->type == t1 && b->type == t2) || (a->type == t2 && b->type == t1))

void fh_while(js_val *, ast_node *, ast_node *);
void fh_for(js_val *, ast_node *, ast_node *);
void fh_forin(js_val *, ast_node *);
void fh_do_assign(js_val *, char *, js_val *, char *);
void fh_func_decl_scan(js_val *, ast_node *);
void fh_var_decl_scan(js_val *, ast_node *);

js_args * fh_build_args(js_val *, ast_node *);
js_val * fh_break(void);
js_val * fh_eval(js_val *, ast_node *);
js_val * fh_eval_each(js_val *, ast_node *);
js_val * fh_exp(js_val *, ast_node *);
js_val * fh_if(js_val *, ast_node *);
js_val * fh_switch(js_val *, ast_node *);
js_val * fh_assign(js_val *, ast_node *);
js_val * fh_return(js_val *, ast_node *);
js_val * fh_prop_lst(js_val *, ast_node *);
js_val * fh_stmt_lst(js_val *, ast_node *);
js_val * fh_src_lst(js_val *, ast_node *);
js_val * fh_new_exp(js_val *, ast_node *);
js_val * fh_obj(js_val *, ast_node *);
js_val * fh_arr(js_val *, ast_node *);
js_val * fh_call_exp(js_val *, ast_node *);
js_val * fh_var_dec(js_val *, ast_node *);
js_val * fh_call(js_val *, js_val *, eval_state *, js_val *, js_args *);
js_val * fh_setup_call_env(js_val *, js_val *, js_val *, js_args *);
js_val * fh_str_from_node(js_val *, ast_node *);
js_val * fh_member(js_val *, ast_node *);
js_val * fh_member_parent(js_val *, ast_node *);
js_val * fh_member_child(js_val *, ast_node *);
js_val * fh_postfix_exp(js_val *, ast_node *);
js_val * fh_prefix_exp(js_val *, ast_node *);
js_val * fh_delete(js_val *, ast_node *);
js_val * fh_bin_exp(js_val *, ast_node *);
js_val * fh_add(js_val *, js_val *);
js_val * fh_sub(js_val *, js_val *);
js_val * fh_mul(js_val *, js_val *);
js_val * fh_div(js_val *, js_val *);
js_val * fh_mod(js_val *, js_val *);
js_val * fh_eq(js_val *, js_val *, bool);
js_val * fh_neq(js_val *, js_val *, bool);
js_val * fh_abstr_rel_comp(js_val *, js_val *, bool);
js_val * fh_lt(js_val *, js_val *, bool);
js_val * fh_gt(js_val *, js_val *, bool);
js_val * fh_and(js_val *, ast_node *, ast_node *);
js_val * fh_or(js_val *, ast_node *, ast_node *);

#endif
