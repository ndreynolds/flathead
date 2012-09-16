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

#include <stdio.h>
#include "flathead.h"

enum NodeType {
  NODE_IDENT,
  NODE_NUM,
  NODE_BOOL,
  NODE_STR,
  NODE_NULL,
  NODE_ASGN,
  NODE_VAR_STMT,
  NODE_EMPT_STMT,
  NODE_EXP_STMT,
  NODE_EXP,
  NODE_STMT_LST,
  NODE_IF,
  NODE_WHILE,
  NODE_DOWHILE,
  NODE_FOR,
  NODE_RETURN,
  NODE_CONT,
  NODE_BREAK,
  NODE_THIS,
  NODE_BLOCK,
  NODE_ARR,
  NODE_EL_LST,
  NODE_ELISION,
  NODE_OBJ,
  NODE_PROP,
  NODE_PROP_LST,
  NODE_UNARY_POST,
  NODE_UNARY_PRE,
  NODE_CALL,
  NODE_NEW,
  NODE_ARG_LST,
  NODE_MEMBER,
  NODE_FUNC,
  NODE_FUNC_DECL,
  NODE_PARAM_LST,
  NODE_SRC_LST,
  NODE_UNKNOWN
};

typedef struct Node {
  struct Node *e1;
  struct Node *e2;
  struct Node *e3;
  char *sval;
  double val;
  enum NodeType type;
  enum NodeType sub_type;
  bool visited;
  int line;
  int column;
} Node;

Node * alloc_node(void);
Node * new_node(enum NodeType, Node *, Node *, Node *, double, 
                char *, int, int);
Node * pop_node(Node *);
void rewind_node(Node *);
bool empty_node(Node *);

void print_indent(int); 
void print_node(Node *, bool, int);

#endif
