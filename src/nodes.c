// nodes.c
// -------

#include "nodes.h"

JLNode *
alloc_node()
{
  // Allocate and return a new node
  struct JLNode *node = malloc(sizeof(struct JLNode));
  node->type = NODE_UNKNOWN;
  node->sub_type = NODE_UNKNOWN;
  node->visited = false;
  return node;
}

JLNode *
new_node(JLNodeType type, JLNode *e1, JLNode *e2, JLNode *e3, double x, char *s)
{
  JLNode *node = alloc_node();

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

  if (type == NODE_NUM || type == NODE_BOOL) node->val = x;
  if (s != 0) {
    node->sval = (char *) malloc(100);
    strcpy(node->sval, s);
  }
  return node;
}

JLNode *
pop_node(JLNode *node)
{
  // Several of our AST nodes follow the pattern:
  //  
  //  Head Tail
  //       Head Tail
  //            Head Tail
  //                 Head `
  // 
  // ...where "Head" is e1 and "Tail" is e2.
  
  if (node->e2 != 0 && !node->e2->visited)
    return pop_node(node->e2);
  node->visited = true;
  return node->e1 != 0 ? node->e1 : NULL;
}

bool
empty_node(JLNode *node)
{
  return node->visited;
}

void
rewind_node(JLNode *node)
{
  // Unset visited on a node linked list.
  node->visited = false;
  if (node->e2 != 0) rewind_node(node->e2);
}

void
print_indent(int indent) 
{
  int i;
  for (i=0; i<indent; i++) printf(" ");
}

void 
print_node(JLNode *node, bool rec, int depth)
{
  print_indent(depth); 
  switch(node->type)
  {
    case NODE_IDENT:
      printf("identifier (%s)\n", node->sval);
      return;
    case NODE_NUM:
      printf("number (%f)\n", node->val);
      return;
    case NODE_BOOL:
      printf("bool (%d)\n", (int)node->val);
      return;
    case NODE_STR:
      printf("string (%s)\n", node->sval);
      return;
    case NODE_NULL:
      printf("null (NULL)\n");
      break;
    case NODE_VAR_STMT:
      printf("variable statement\n");
      break;
    case NODE_EXP_STMT:
      printf("expression statement\n");
      break;
    case NODE_EXP:
      printf("expression\n");
      break;
    case NODE_STMT_LST:
      printf("statement list\n");
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
      break;
    case NODE_BLOCK:
      printf("block\n");
      break;
    case NODE_OBJ:
      printf("object\n");
      break;
    case NODE_PROP:
      printf("property\n");
      break;
    case NODE_PROP_LST:
      printf("property list\n");
      break;
    case NODE_EMPT_STMT:
      printf("empty statement\n");
      break;
    case NODE_CALL:
      printf("call expression\n");
      break;
    case NODE_NEW:
      printf("new expression\n");
      break;
    case NODE_ARG_LST:
      printf("argument list\n");
      break;
    case NODE_MEMBER:
      printf("member expression\n");
      break;
    case NODE_FUNC:
      printf("function expression\n");
      break;
    case NODE_FUNC_DECL:
      printf("function declaration\n");
      break;
    case NODE_PARAM_LST:
      printf("parameter list\n");
      break;
    default:
      printf("unknown type: %d\n", node->type);
  }

  if (!rec) return;
  if (node->sval != 0) {
    print_indent(depth+2);
    printf("%s\n", node->sval);
  }
  if (node->e1 != 0) print_node(node->e1, rec, depth+2);
  if (node->e2 != 0) print_node(node->e2, rec, depth+2);
  if (node->e3 != 0) print_node(node->e3, rec, depth+2);
}
