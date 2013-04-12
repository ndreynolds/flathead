/*
 * nodes.h -- AST node definitions and traversal helpers
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

#ifndef NODES_H
#define NODES_H

#include <stdbool.h>

enum ast_node_type {
  NODE_ARG_LST,
  NODE_ARR,
  NODE_ASGN,
  NODE_BLOCK,
  NODE_BOOL,
  NODE_BREAK,
  NODE_CALL,
  NODE_CASE_BLOCK,
  NODE_CATCH,
  NODE_CLAUSE,
  NODE_CLAUSE_LST,
  NODE_CONT,
  NODE_DEBUG_STMT,
  NODE_DOWHILE,
  NODE_ELISION,
  NODE_EL_LST,
  NODE_EMPT_STMT,
  NODE_EXP,
  NODE_EXPGRP,
  NODE_EXP_STMT,
  NODE_FINALLY,
  NODE_FOR,
  NODE_FORIN,
  NODE_FUNC,
  NODE_IDENT,
  NODE_IF,
  NODE_MEMBER,
  NODE_NEW,
  NODE_NULL,
  NODE_NUM,
  NODE_OBJ,
  NODE_PARAM_LST,
  NODE_PROP,
  NODE_PROP_LST,
  NODE_REGEXP,
  NODE_RETURN,
  NODE_SRC_LST,
  NODE_STMT_LST,
  NODE_STR,
  NODE_SWITCH_STMT,
  NODE_TERN,
  NODE_THIS,
  NODE_THROW,
  NODE_TRY_STMT,
  NODE_UNARY_POST,
  NODE_UNARY_PRE,
  NODE_UNKNOWN,
  NODE_VAR_DEC,
  NODE_VAR_DEC_LST,
  NODE_VAR_STMT,
  NODE_WITH_STMT,
  NODE_WHILE,
};

typedef struct ast_node {
  struct ast_node *e1;
  struct ast_node *e2;
  struct ast_node *e3;
  char *sval;
  double val;
  enum ast_node_type type;
  enum ast_node_type sub_type;
  bool visited;
  int line;
  int column;
} ast_node;

ast_node * alloc_node(void);
ast_node * new_node(enum ast_node_type, ast_node *, ast_node *, ast_node *, double, 
                char *, int, int);
ast_node * pop_node(ast_node *);
void rewind_node(ast_node *);
bool empty_node(ast_node *);
int count_node(ast_node *);

void print_indent(int); 
void print_node(ast_node *, bool, int);

#endif
