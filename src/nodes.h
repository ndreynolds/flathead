// nodes.h
// -------

#ifndef NODES_H
#define NODES_H

#include <stdio.h>
#include "jslite.h"

typedef enum JLNodeType {
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
  NODE_OBJ,
  NODE_PROP,
  NODE_PROP_LST,
  NODE_EL_LST,
  NODE_UNARY_POST,
  NODE_UNARY_PRE,
  NODE_UNKNOWN
} JLNodeType;

typedef struct JLNode {
  struct JLNode *e1;
  struct JLNode *e2;
  struct JLNode *e3;
  char *sval;
  double val;
  JLNodeType type;
  JLNodeType sub_type;
} JLNode;

JLNode * alloc_node(void);
JLNode * new_node(JLNodeType, JLNode *, JLNode *, JLNode *, double, char *);

#define NEW_IDENT(name)         new_node(NODE_IDENT,0,0,0,0,name)
#define NEW_VARSTMT(ident)      new_node(NODE_VAR_STMT,ident,0,0,0,0)
#define NEW_WHILE(cnd,blck)     new_node(NODE_WHILE,cnd,blck,0,0,0)
#define NEW_DOWHILE(cnd,blck)   new_node(NODE_DOWHILE,cnd,blck,0,0,0)
#define NEW_BLOCK(stmtlst)      new_node(NODE_BLOCK,stmtlst,0,0,0,0)
#define NEW_STMTLST(head,tail)  new_node(NODE_STMT_LST,head,tail,0,0,0)
#define NEW_IF(cnd,ifb,elseb)   new_node(NODE_IF,cnd,ifb,elseb,0,0)
#define NEW_FOR(init,cnd,post)  new_node(NODE_FOR,init,cnd,post,0,0)
#define NEW_NUM(x)              new_node(NODE_NUM,0,0,0,x,0)
#define NEW_BOOL(x)             new_node(NODE_BOOL,0,0,0,x,0)
#define NEW_STR(x)              new_node(NODE_STR,0,0,0,0,x)
#define NEW_NULL()              new_node(NODE_NULL,0,0,0,0,0)
#define NEW_RETURN(exp)         new_node(NODE_RETURN,exp,0,0,0,0)
#define NEW_CONT()              new_node(NODE_CONT,0,0,0,0,0)
#define NEW_BREAK()             new_node(NODE_BREAK,0,0,0,0,0)
#define NEW_THIS()              new_node(NODE_THIS,0,0,0,0,0)
#define NEW_EMPTSTMT()          new_node(NODE_EMPT_STMT,0,0,0,0,0)
#define NEW_EXP(a,b,op)         new_node(NODE_EXP,a,b,0,0,op)
#define NEW_UNPOST(a,op)        new_node(NODE_UNARY_POST,a,0,0,0,op)
#define NEW_UNPRE(a,op)         new_node(NODE_UNARY_PRE,a,0,0,0,op)
#define NEW_EXPSTMT(exp)        new_node(NODE_EXP_STMT,exp,0,0,0,0)
#define NEW_ASGN(a,b,op)        new_node(NODE_ASGN,a,b,0,0,op)
#define NEW_ARR()               new_node(NODE_ARR,0,0,0,0,0)
#define NEW_OBJ(proplst)        new_node(NODE_OBJ,proplst,0,0,0,0)
#define NEW_PROP(name,exp)      new_node(NODE_PROP,name,exp,0,0,0)
#define NEW_PROPLST(head,tail)  new_node(NODE_PROP_LST,head,tail,0,0,0)

void print_indent(int); 
void print_node(JLNode *, bool, int);

#endif
