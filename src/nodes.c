/*
 * nodes.c -- AST node definitions and traversal helpers
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nodes.h"

Node *
alloc_node()
{
  // Allocate and return a new node
  struct Node *node = malloc(sizeof(struct Node));
  node->type = NODE_UNKNOWN;
  node->sub_type = NODE_UNKNOWN;
  node->visited = false;
  return node;
}

Node *
new_node(enum NodeType type, Node *e1, Node *e2, Node *e3, 
         double x, char *s, int line, int column)
{
  Node *node = alloc_node();

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

  if (type == NODE_NUM || type == NODE_BOOL || type == NODE_MEMBER) 
    node->val = x;

  node->sval = NULL;
  if (s != NULL) {
    node->sval = malloc((strlen(s) + 1) * sizeof(char));
    strcpy(node->sval, s);
  }
  return node;
}

Node *
pop_node(Node *node)
{
  if (node->e2 != NULL && !node->e2->visited)
    return pop_node(node->e2);
  node->visited = true;
  return node->e1;
}

bool
empty_node(Node *node)
{
  return node->visited;
}

void
rewind_node(Node *node)
{
  // Unset visited on a node linked list.
  if (node->e1 == 0) return;
  node->visited = false;
  if (node->e2 != NULL) rewind_node(node->e2);
}

void
print_indent(int indent) 
{
  int i;
  for (i = 0; i < indent; i++) printf(" ");
}

void 
print_node(Node *node, bool rec, int depth)
{
  print_indent(depth); 
  switch(node->type) {
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
    print_indent(depth+2);
    printf("%s\n", node->sval);
  }
  if (node->e1 != NULL) print_node(node->e1, rec, depth + 2);
  if (node->e2 != NULL) print_node(node->e2, rec, depth + 2);
  if (node->e3 != NULL) print_node(node->e3, rec, depth + 2);
}
