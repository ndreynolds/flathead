#include <stdio.h>
#include <stdlib.h>
#include "jslite.h"

enum JLAssignmentOperator {
  MINUS_EQ,
  PLUS_EQ,
  TIMES_EQ,
  DIV_EQ
};

enum JLNodeType {
  NODE_IDENT,
  NODE_NUM,
  NODE_BOOL,
  NODE_STR,
  NODE_NULL,
  NODE_ASGN,
  NODE_ASGN_EXP,
  NODE_VAR_STMT,
  NODE_EMPT_STMT,
  NODE_EXP_STMT,
  NODE_EXP,
  NODE_STMT_LST,
  NODE_IF,
  NODE_WHILE,
  NODE_DOWHILE,
  NODE_FOR,
  NODE_CTL,
  NODE_THIS,
  NODE_BLOCK,
  NODE_ARR,
  NODE_EL_LST,
  NODE_UNKNOWN
};

struct JLIdentifierNode {
  char *value;
};

struct JLNumberNode {
  double value;
};

struct JLBooleanNode {
  bool value;
};

struct JLStringNode {
  char *value;
};

struct JLNullNode {
  // null
};

struct JLStatementNode {
  union {
    struct JLVariableStatementNode *varstmt;
    struct JLExpressionStatementNode *expstmt;
    struct JLIfStatementNode *ifstmt;
    struct JLForStatementNode *forstmt;
    struct JLDoWhileStatementNode *dowhilestmt;
    struct JLWhileStatementNode *whilestmt;
  } as;
};

/**
 * Assigns the value of an expression to a variable.
 *
 * x = 42;
 * x += 42;
 */
struct JLAssignmentNode {
  char *oper;
  struct JLIdentifierNode *identifier;
  struct JLExpressionNode *expression;
};

/**
 * Holds an expression (binary or unary)
 */
struct JLExpressionNode {
  char *oper;
  struct JLNode *leadingOperand;
  struct JLNode *trailingOperand;
};

/**
 * Initializes a variable. Only one per `var` in JSLite.
 *
 *  var [ident];
 */
struct JLVariableStatementNode {
  struct JLIdentifierNode *identifier;
};

/**
 * A while loop
 *
 *  while([exp]) {
 *    [block]
 *  }
 */
struct JLWhileNode {
  struct JLExpressionNode *condition;
  struct JLStatementNode *whileBlock;
};

/**
 * A do-while loop
 *
 *  do {
 *    [block]
 *  } while([exp]);
 */
struct JLDoWhileNode {
  struct JLExpressionNode *condition;
  struct JLStatementNode *whileBlock;
};

struct JLBlockNode {
  struct JLStatementListNode *stmtlst;
};

struct JLStatementListNode {
  struct JLStatementNode *head;
  struct JLStatementListNode *tail;
};

struct JLExpressionStatementNode {
  struct JLExpressionNode *exp;
};

/**
 * A for loop
 *
 *  for([exp]; [exp]; [exp]) {
 *    [block]
 *  }
 */
struct JLForNode {
  struct JLInitializationNode *initialization;
  struct JLExpressionNode *condition;
  struct JLExpressionNode *postExpression;
  struct JLStatementNode *forBlock;
};

struct JLIfNode {
  struct JLExpressionNode *condition;
  struct JLStatementNode *ifBlock;
  struct JLStatementNode *elseBlock;
};

struct JLNode {
  union {
    struct JLIdentifierNode identifiern;
    struct JLVariableStatementNode varstmtn;
    struct JLWhileNode whilen;
    struct JLBlockNode blockn;
    struct JLStatementListNode stmtlstn;
    struct JLStatementNode stmtn;
    struct JLForNode forn;
    struct JLIfNode ifn;
    struct JLNumberNode numn;
    struct JLStringNode strn;
    struct JLBooleanNode booln;
    struct JLIdentifierNode identn;
    struct JLExpressionStatementNode expstmtn;
    struct JLExpressionNode expn;
    struct JLDoWhileNode dowhilen;
  } as;
  enum JLNodeType type;
};

struct JLNode *
alloc_node()
{
  // Allocate and return a new node
  struct JLNode *node = malloc(sizeof(struct JLNode));
  node->type = NODE_UNKNOWN;
  return node;
}

struct JLNode *
if_node(struct JLNode *cond, struct JLNode *ifBlock, struct JLNode *elseBlock)
{
  struct JLNode *node = alloc_node();
  node->as.ifn.condition = (struct JLExpressionNode *) cond;
  node->as.ifn.ifBlock = (struct JLStatementNode *) ifBlock;
  node->as.ifn.elseBlock = (struct JLStatementNode *) elseBlock;
  node->type = NODE_IF;
  return node;
}

struct JLNode *
num_node(double num)
{
  struct JLNode *node = alloc_node();
  node->as.numn.value = num;
  node->type = NODE_NUM;
  return node;
}

struct JLNode *
block_node(struct JLNode *stmtlst)
{
  struct JLNode *node = alloc_node();
  node->as.blockn.stmtlst = (struct JLStatementListNode *) stmtlst;
  node->type = NODE_BLOCK;
  return node;
}

struct JLNode *
stmtlst_node(struct JLNode *stmt, struct JLNode *stmt_list)
{
  struct JLNode *node = alloc_node();
  node->as.stmtlstn.head = (struct JLStatementNode *) stmt;
  node->as.stmtlstn.tail = (struct JLStatementListNode *) stmt_list;
  node->type = NODE_STMT_LST;
  return node;
}

struct JLNode *
varstmt_node(struct JLNode *ident) 
{
  struct JLNode *node = alloc_node();
  node->as.varstmtn.identifier = (struct JLIdentifierNode *) ident;
  node->type = NODE_VAR_STMT;
  return node;
}

struct JLNode *
emptstmt_node()
{
  struct JLNode *node = alloc_node();
  node->type = NODE_EMPT_STMT;
  return node;
}

struct JLNode *
expstmt_node(struct JLNode *exp)
{
  struct JLNode *node = alloc_node();
  node->as.expstmtn.exp = (struct JLExpressionNode *) exp;
  node->type = NODE_EXP_STMT;
  return node;
}

struct JLNode *
dowhile_node(struct JLNode *stmt, struct JLNode *cond)
{
  struct JLNode *node = alloc_node();
  node->as.dowhilen.whileBlock = (struct JLStatementNode *) stmt;
  node->as.dowhilen.condition = (struct JLExpressionNode *) cond;
  node->type = NODE_DOWHILE;
  return node;
}

struct JLNode *
while_node(struct JLNode *cond, struct JLNode *stmt)
{
  struct JLNode *node = alloc_node();
  node->as.whilen.condition = (struct JLExpressionNode *) cond;
  node->as.whilen.whileBlock = (struct JLStatementNode *) stmt;
  node->type = NODE_WHILE;
  return node;
}

struct JLNode *
for_node()
{
  struct JLNode *node = alloc_node();
  // stub
  node->type = NODE_FOR;
  return node;
}

struct JLNode *
exp_node(char *oper, struct JLNode *lead_operand, struct JLNode *trail_operand)
{
  struct JLNode *node = alloc_node();
  node->as.expn.oper = oper;
  node->as.expn.leadingOperand = lead_operand;
  node->as.expn.trailingOperand = trail_operand;
  node->type = NODE_EXP;
  return node;
}

struct JLNode *
ctl_node(struct JLNode *exp)
{
  struct JLNode *node = alloc_node();
  // stub
  node->type = NODE_CTL;
  return node;
}

struct JLNode *
asgn_node()
{
  struct JLNode *node = alloc_node();
  // stub
  node->type = NODE_ASGN;
  return node;
}

struct JLNode *
asgnexp_node()
{
  struct JLNode *node = alloc_node();
  // stub
  node->type = NODE_ASGN_EXP;
  return node;
}

struct JLNode *
bool_node(bool truth_value)
{
  struct JLNode *node = alloc_node();
  node->as.booln.value = truth_value;
  node->type = NODE_BOOL;
  return node;
}

struct JLNode *
null_node()
{
  struct JLNode *node = alloc_node();
  node->type = NODE_NULL;
  return node;
}

struct JLNode *
this_node()
{
  struct JLNode *node = alloc_node();
  // stub
  node->type = NODE_THIS;
  return node;
}

struct JLNode *
ident_node()
{
  struct JLNode *node = alloc_node();
  // stub
  node->type = NODE_IDENT;
  return node;
}

struct JLNode *
str_node(char *val)
{
  struct JLNode *node = alloc_node();
  node->as.strn.value = val;
  printf("%p\n", val);
  printf("%p\n", node->as.strn.value);
  node->type = NODE_STR;
  return node;
}

struct JLNode *
arr_node()
{
  struct JLNode *node = alloc_node();
  // stub
  node->type = NODE_ARR;
  return node;
}

struct JLNode *
ellst_node()
{
  struct JLNode *node = alloc_node();
  // stub
  node->type = NODE_EL_LST;
  return node;
}

void
print_indent(int indent) 
{
  int i;
  for (i=0; i<indent; i++) printf("-");
  if (indent > 0) printf(" ");
}

void 
print_node(struct JLNode *node, bool rec, int depth)
{
  print_indent(depth);
  switch(node->type)
  {
    case NODE_IDENT:
      printf("identifier (%s)\n", node->as.identn.value);
      break;
    case NODE_NUM:
      printf("number (%f)\n", node->as.numn.value);
      break;
    case NODE_BOOL:
      printf("bool (%d)\n", node->as.booln.value);
      break;
    case NODE_STR:
      printf("string (%p)\n", &node->as.strn.value);
      break;
    case NODE_NULL:
      printf("null (NULL)\n");
      break;
    case NODE_VAR_STMT:
      printf("variable statement\n");
      if (rec) print_node((struct JLNode *)node->as.varstmtn.identifier, rec, depth+2);
      break;
    case NODE_EXP_STMT:
      printf("expression statement\n");
      if (rec) print_node((struct JLNode *)node->as.expstmtn.exp, rec, depth+2);
      break;
    case NODE_EXP:
      printf("expression\n");
      if (rec) {
        print_indent(depth+2);
        printf("%s\n", node->as.expn.oper);
        print_node((struct JLNode *)node->as.expn.leadingOperand, rec, depth+2);
        print_node((struct JLNode *)node->as.expn.trailingOperand, rec, depth+2);
      }
      break;
    case NODE_STMT_LST:
      printf("statement list\n");
      if (rec) {
        print_node((struct JLNode *)node->as.stmtlstn.head, rec, depth+2);
        print_node((struct JLNode *)node->as.stmtlstn.tail, rec, depth+2);
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
    case NODE_CTL:
      printf("control\n");
      break;
    case NODE_THIS:
      printf("this\n");
      break;
    case NODE_BLOCK:
      printf("block\n");
      break;
    default:
      printf("unknown type\n");
  }
}
