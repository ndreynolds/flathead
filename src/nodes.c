/*
 * nodes.c -- AST node definitions and traversal helpers
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nodes.h"

ast_node *
node_alloc()
{
  // Allocate and return a new node
  struct ast_node *node = calloc(1, sizeof(*node));
  node->type = NODE_UNKNOWN;
  node->sub_type = NODE_UNKNOWN;
  node->visited = false;
  return node;
}

ast_node *
node_new(enum ast_node_type type, ast_node *e1, ast_node *e2, ast_node *e3, 
         double x, char *s, int line, int column)
{
  ast_node *node = node_alloc();

  // Assign expression subtypes
  if (type == NODE_UNARY_POST || type == NODE_UNARY_PRE) {
    node->sub_type = type;
    node->type = NODE_EXP;
    type = NODE_EXP;
  }
  else {
    node->type = type;
  }

  node->e1 = e1;
  node->e2 = e2;
  node->e3 = e3;

  node->line = line;
  node->column = column;

  node->val = 0;
  if (type == NODE_NUM || type == NODE_BOOL || type == NODE_MEMBER) 
    node->val = x;

  node->sval = NULL;
  if (s != NULL) {
    node->sval = malloc((strlen(s) + 1) * sizeof(char));
    strcpy(node->sval, s);
    node->sval[strlen(s)] = '\0';
  }
  return node;
}

ast_node *
node_pop(ast_node *node)
{
  if (node->e2 != NULL && !node->e2->visited)
    return node_pop(node->e2);
  node->visited = true;
  return node->e1;
}

bool
node_empty(ast_node *node)
{
  return node->visited;
}

void
node_rewind(ast_node *node)
{
  // Unset visited on a node linked list.
  if (!node->e1) return;
  node->visited = false;
  if (node->e2 != NULL) node_rewind(node->e2);
}

int
node_count(ast_node *node)
{
  int count = 0;
  ast_node *head = node;
  while (head) {
    if (head->e1) count++;
    head = head->e2;
  }
  return count;
}

void 
node_print(ast_node *node, bool rec, int depth)
{
  if (depth) printf("%*s", depth, " ");
  switch (node->type) {
    case NODE_ARG_LST:     printf("argument list"); break;
    case NODE_ARR:         printf("array literal"); break;
    case NODE_ASGN:        printf("assignment"); break;
    case NODE_BLOCK:       printf("block"); break;
    case NODE_BREAK:       printf("break"); break;
    case NODE_CALL:        printf("call expression"); break;
    case NODE_CASE_BLOCK:  printf("case block"); break;
    case NODE_CATCH:       printf("catch"); break;
    case NODE_CLAUSE:      printf("case clause"); break;
    case NODE_CLAUSE_LST:  printf("case clause list"); break;
    case NODE_CONT:        printf("continue"); break;
    case NODE_DOWHILE:     printf("dowhile"); break;
    case NODE_ELISION:     printf("elision"); break;
    case NODE_EL_LST:      printf("element list"); break;
    case NODE_EMPT_STMT:   printf("empty statement"); break;
    case NODE_EXPGRP:      printf("expression group"); break;
    case NODE_EXP_STMT:    printf("expression statement"); break;
    case NODE_FINALLY:     printf("finally"); break;
    case NODE_FOR:         printf("for"); break;
    case NODE_FORIN:       printf("for-in"); break;
    case NODE_FUNC:        printf("function"); break;
    case NODE_IF:          printf("if"); break;
    case NODE_MEMBER:      printf("member expression"); break;
    case NODE_NEW:         printf("new expression"); break;
    case NODE_OBJ:         printf("object"); break;
    case NODE_PARAM_LST:   printf("parameter list"); break;
    case NODE_PROP:        printf("property"); break;
    case NODE_PROP_LST:    printf("property list"); break;
    case NODE_RETURN:      printf("return"); break;
    case NODE_SRC_LST:     printf("source list"); break;
    case NODE_STMT_LST:    printf("statement list"); break;
    case NODE_SWITCH_STMT: printf("switch statement"); break;
    case NODE_TERN:        printf("conditional expression"); break;
    case NODE_THIS:        printf("this"); break;
    case NODE_THROW:       printf("throw"); break;
    case NODE_TRY_STMT:    printf("try statement"); break;
    case NODE_VAR_DEC:     printf("variable declaration"); break;
    case NODE_VAR_DEC_LST: printf("variable declaration list"); break;
    case NODE_VAR_STMT:    printf("variable statement"); break;
    case NODE_WHILE:       printf("while"); break;
    case NODE_WITH_STMT:   printf("with statement"); break;

    // Literals
    case NODE_BOOL:        printf("bool (%d)\n", (int)node->val); return;
    case NODE_IDENT:       printf("identifier (%s)\n", node->sval); return;
    case NODE_NULL:        printf("null (NULL)\n"); return;
    case NODE_NUM:         printf("number (%f)\n", node->val); return;
    case NODE_REGEXP:      printf("regexp (%s)\n", node->sval); return;
    case NODE_STR:         printf("string (%s)\n", node->sval); return;

    // Expressions
    case NODE_EXP:
      printf("expression ");
      if (node->sub_type == NODE_UNARY_PRE) printf("(unary prefix)");
      else if (node->sub_type == NODE_UNARY_POST) printf("(unary postfix)");
      else printf ("(binary)");
      break;

    default:
      printf("unknown type: %d", node->type);
  }
  printf("\n");
  if (!rec) return;
  if (node->sval) {
    printf("%*s%s\n", depth+2, " ", node->sval);
  }
  if (node->e1 != NULL) node_print(node->e1, rec, depth + 2);
  if (node->e2 != NULL) node_print(node->e2, rec, depth + 2);
  if (node->e3 != NULL) node_print(node->e3, rec, depth + 2);
}
