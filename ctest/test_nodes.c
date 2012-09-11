// test_nodes.c
// ------------
// Tests for the AST node representation and utilities.

#include "test.h"
#include "../src/flathead.h"
#include "../src/nodes.c"

#define NEW_NODE(t,e1,e2,e3,d,s)   new_node(t,e1,e2,e3,d,s,0,0) 
#define NEW_NUM(x)                 NEW_NODE(NODE_NUM,0,0,0,x,0)
#define NEW_STMTLST(head,tail)     NEW_NODE(NODE_STMT_LST,head,tail,0,0,0)

void
test_pop_node_returns_the_last_node_in_a_linked_list()
{
  Node *a = NEW_STMTLST(NEW_NUM(1), NULL);
  Node *b = NEW_STMTLST(NEW_NUM(2), a);
  Node *c = NEW_STMTLST(NEW_NUM(3), b);
  Node *d = NEW_STMTLST(NEW_NUM(4), c);

  Node *found1 = pop_node(d);
  TEST(found1->val == 1);

  Node *found2 = pop_node(d);
  TEST(found2->val == 2);

  Node *found3 = pop_node(d);
  TEST(found3->val == 3);

  Node *found4 = pop_node(d);
  TEST(found4->val == 4);
}

void
test_rewind_node_resets_a_previous_sequence_of_pops()
{
  Node *a = NEW_STMTLST(NEW_NUM(1), NULL);
  Node *b = NEW_STMTLST(NEW_NUM(2), a);
  Node *c = NEW_STMTLST(NEW_NUM(3), b);
  Node *d = NEW_STMTLST(NEW_NUM(4), c);

  Node *found1 = pop_node(d);
  TEST(found1->val == 1);

  Node *found2 = pop_node(d);
  TEST(found2->val == 2);

  Node *found3 = pop_node(d);
  TEST(found3->val == 3);

  Node *found4 = pop_node(d);
  TEST(found4->val == 4);

  // Now rewind, and start over.
  rewind_node(d);

  Node *found5 = pop_node(d);
  TEST(found5->val == 1);

  Node *found6 = pop_node(d);
  TEST(found6->val == 2);

  Node *found7 = pop_node(d);
  TEST(found7->val == 3);

  Node *found8 = pop_node(d);
  TEST(found8->val == 4);
}

void
test_empty_returns_true_on_an_emptied_list()
{
  Node *a = NEW_STMTLST(NEW_NUM(1), NULL);
  Node *b = NEW_STMTLST(NEW_NUM(2), a);
  Node *c = NEW_STMTLST(NEW_NUM(3), b);
  Node *d = NEW_STMTLST(NEW_NUM(4), c);

  TEST(empty_node(d) == false);

  Node *found1 = pop_node(d);
  Node *found2 = pop_node(d);
  Node *found3 = pop_node(d);
  TEST(empty_node(d) == false);
  Node *found4 = pop_node(d);
  TEST(found4->val == 4);

  TEST(empty_node(d) == true);
}

int
main()
{
  TEST_START("Node traversal tests");

  test_pop_node_returns_the_last_node_in_a_linked_list();
  test_rewind_node_resets_a_previous_sequence_of_pops();
  test_empty_returns_true_on_an_emptied_list();
  
  TEST_SUMMARY();

  return 0;
}
