#include <stdlib.h>
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
  NODE_EL_LST,
  NODE_UNKNOWN
} JLNodeType;

typedef struct JLNode {
  struct JLNode *e1;
  struct JLNode *e2;
  struct JLNode *e3;
  char *sval;
  double val;
  JLNodeType type;
} JLNode;

JLNode *
alloc_node()
{
  // Allocate and return a new node
  struct JLNode *node = malloc(sizeof(struct JLNode));
  node->type = NODE_UNKNOWN;
  return node;
}

JLNode *
new_node(JLNodeType type, JLNode *e1, JLNode *e2, JLNode *e3, double x, char *s)
{
  JLNode *node = alloc_node();
  node->type = type;
  node->e1 = e1;
  node->e2 = e2;
  node->e3 = e3;
  if (type == NODE_NUM || type == NODE_BOOL) node->val = x;
  if (type == NODE_STR || type == NODE_EXP || type == NODE_ASGN) {
    node->sval = (char *) malloc(100);
    strcpy(node->sval, s);
  }
  return node;
}

#define NEW_IDENT(name)         new_node(NODE_IDENT,0,0,0,0,name)
#define NEW_VARSTMT(ident)      new_node(NODE_VAR_STMT,ident,0,0,0,"")
#define NEW_WHILE(cnd,blck)     new_node(NODE_WHILE,cnd,blck,0,0,"")
#define NEW_DOWHILE(cnd,blck)   new_node(NODE_DOWHILE,cnd,blck,0,0,"")
#define NEW_BLOCK(stmtlst)      new_node(NODE_BLOCK,stmtlst,0,0,0,"")
#define NEW_STMTLST(head,tail)  new_node(NODE_STMT_LST,head,tail,0,0,"")
#define NEW_IF(cnd,ifb,elseb)   new_node(NODE_IF,cnd,ifb,elseb,0,"")
#define NEW_FOR(init,cnd,post)  new_node(NODE_FOR,init,cnd,post,0,"")
#define NEW_NUM(x)              new_node(NODE_NUM,0,0,0,x,"")
#define NEW_BOOL(x)             new_node(NODE_BOOL,0,0,0,x,"")
#define NEW_STR(x)              new_node(NODE_STR,0,0,0,0,x)
#define NEW_NULL()              new_node(NODE_NULL,0,0,0,0,"")
#define NEW_RETURN(exp)         new_node(NODE_RETURN,exp,0,0,0,"")
#define NEW_CONT()              new_node(NODE_CONT,0,0,0,0,"")
#define NEW_BREAK()             new_node(NODE_BREAK,0,0,0,0,"")
#define NEW_THIS()              new_node(NODE_THIS,0,0,0,0,"")
#define NEW_EMPTSTMT()          new_node(NODE_EMPT_STMT,0,0,0,0,"")
#define NEW_EXP(a,b,op)         new_node(NODE_EXP,a,b,0,0,op)
#define NEW_UNEXP(a,op)         new_node(NODE_EXP,a,0,0,0,op)
#define NEW_EXPSTMT(exp)        new_node(NODE_EXP_STMT,exp,0,0,0,"")
#define NEW_ASGN(a,b,op)        new_node(NODE_ASGN,a,b,0,0,op)
#define NEW_ARR()               new_node(NODE_ARR,0,0,0,0,"")

void
print_indent(int indent) 
{
  int i;
  for (i=0; i<indent; i++) printf("-");
  if (indent > 0) printf(" ");
}

void 
print_node(JLNode *node, bool rec, int depth)
{
  print_indent(depth);
  switch(node->type)
  {
    case NODE_IDENT:
      printf("identifier (%s)\n", node->sval);
      break;
    case NODE_NUM:
      printf("number (%f)\n", node->val);
      break;
    case NODE_BOOL:
      printf("bool (%d)\n", (int)node->val);
      break;
    case NODE_STR:
      printf("string (%s)\n", node->sval);
      break;
    case NODE_NULL:
      printf("null (NULL)\n");
      break;
    case NODE_VAR_STMT:
      printf("variable statement\n");
      if (rec) print_node(node->e1, rec, depth+2);
      break;
    case NODE_EXP_STMT:
      printf("expression statement\n");
      if (rec) print_node(node->e1, rec, depth+2);
      break;
    case NODE_EXP:
      printf("expression\n");
      if (rec) {
        print_indent(depth+2);
        printf("%s\n", node->sval);
        print_node(node->e1, rec, depth+2);
        print_node(node->e2, rec, depth+2);
      }
      break;
    case NODE_STMT_LST:
      printf("statement list\n");
      if (rec) {
        print_node(node->e1, rec, depth+2);
        print_node(node->e2, rec, depth+2);
      }
      break;
    case NODE_ASGN:
      printf("assignment\n");
      break;
    case NODE_IF:
      printf("if\n");
      break;
    case NODE_WHILE:
      printf("while\n");
      break;
    case NODE_DOWHILE:
      printf("dowhile\n");
      break;
    case NODE_FOR:
      printf("for\n");
      break;
    case NODE_BREAK:
      printf("break\n");
      break;
    case NODE_CONT:
      printf("continue\n");
      break;
    case NODE_THIS:
      printf("this\n");
      break;
    case NODE_RETURN:
      printf("return\n");
      if (rec) print_node(node->e1, rec, depth+2);
      break;
    case NODE_BLOCK:
      printf("block\n");
      break;
    default:
      printf("unknown type\n");
  }
}
