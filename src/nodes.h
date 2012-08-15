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

enum JSNodeType {
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
  NODE_UNKNOWN
};

typedef struct JSNODE {
  struct JSNODE *e1;
  struct JSNODE *e2;
  struct JSNODE *e3;
  char *sval;
  double val;
  enum JSNodeType type;
  enum JSNodeType sub_type;
  bool visited;
} JSNODE;

JSNODE * alloc_node(void);
JSNODE * new_node(enum JSNodeType, JSNODE *, JSNODE *, JSNODE *, double, char *);
JSNODE * pop_node(JSNODE *);
void rewind_node(JSNODE *);
bool empty_node(JSNODE *);

#define NEW_IDENT(name)            new_node(NODE_IDENT,0,0,0,0,name)
#define NEW_VARSTMT(ident,exp)     new_node(NODE_VAR_STMT,ident,exp,0,0,0)
#define NEW_WHILE(cnd,blck)        new_node(NODE_WHILE,cnd,blck,0,0,0)
#define NEW_DOWHILE(cnd,blck)      new_node(NODE_DOWHILE,cnd,blck,0,0,0)
#define NEW_BLOCK(stmtlst)         new_node(NODE_BLOCK,stmtlst,0,0,0,0)
#define NEW_STMTLST(head,tail)     new_node(NODE_STMT_LST,head,tail,0,0,0)
#define NEW_IF(cnd,ifb,elseb)      new_node(NODE_IF,cnd,ifb,elseb,0,0)
#define NEW_FOR(init,cnd,post)     new_node(NODE_FOR,init,cnd,post,0,0)
#define NEW_NUM(x)                 new_node(NODE_NUM,0,0,0,x,0)
#define NEW_BOOL(x)                new_node(NODE_BOOL,0,0,0,x,0)
#define NEW_STR(x)                 new_node(NODE_STR,0,0,0,0,x)
#define NEW_NULL()                 new_node(NODE_NULL,0,0,0,0,0)
#define NEW_RETURN(exp)            new_node(NODE_RETURN,exp,0,0,0,0)
#define NEW_CONT()                 new_node(NODE_CONT,0,0,0,0,0)
#define NEW_BREAK()                new_node(NODE_BREAK,0,0,0,0,0)
#define NEW_THIS()                 new_node(NODE_THIS,0,0,0,0,0)
#define NEW_EMPTSTMT()             new_node(NODE_EMPT_STMT,0,0,0,0,0)
#define NEW_EXP(a,b,op)            new_node(NODE_EXP,a,b,0,0,op)
#define NEW_UNPOST(a,op)           new_node(NODE_UNARY_POST,a,0,0,0,op)
#define NEW_UNPRE(a,op)            new_node(NODE_UNARY_PRE,a,0,0,0,op)
#define NEW_EXPSTMT(exp)           new_node(NODE_EXP_STMT,exp,0,0,0,0)
#define NEW_ASGN(a,b,op)           new_node(NODE_ASGN,a,b,0,0,op)
#define NEW_ARR(ellst)             new_node(NODE_ARR,ellst,0,0,0,0)
#define NEW_ELLST(head,tail)       new_node(NODE_EL_LST,head,tail,0,0,0)
#define NEW_OBJ(proplst)           new_node(NODE_OBJ,proplst,0,0,0,0)
#define NEW_PROP(name,exp)         new_node(NODE_PROP,name,exp,0,0,0)
#define NEW_PROPLST(head,tail)     new_node(NODE_PROP_LST,head,tail,0,0,0)
#define NEW_ARGLST(head,tail)      new_node(NODE_ARG_LST,head,tail,0,0,0)
#define NEW_CALL(call,args)        new_node(NODE_CALL,call,args,0,0,0);
#define NEW_NEW(exp)               new_node(NODE_NEW,exp,0,0,0,0);
#define NEW_MEMBER(head,tail)      new_node(NODE_MEMBER,head,tail,0,0,0)
#define NEW_FUNC(params,body,id)   new_node(NODE_FUNC,params,body,id,0,0)
#define NEW_FUNCDL(params,body,id) new_node(NODE_FUNC_DECL,params,body,id,0,0)
#define NEW_PARAMLST(head,tail)    new_node(NODE_PARAM_LST,head,tail,0,0,0)
#define NEW_ELISION()              new_node(NODE_ELISION,0,0,0,0,0)

void print_indent(int); 
void print_node(JSNODE *, bool, int);

#endif
