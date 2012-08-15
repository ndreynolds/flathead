// test_nodes.c
// ------------
// Tests for the AST node representation and utilities.

#include "test.h"
#include "../src/flathead.h"
#include "../src/nodes.c"

void
test_pop_node_returns_the_last_node_in_a_linked_list()
{
  JSNODE *a = NEW_STMTLST(NEW_NUM(1), NULL);
  JSNODE *b = NEW_STMTLST(NEW_NUM(2), a);
  JSNODE *c = NEW_STMTLST(NEW_NUM(3), b);
  JSNODE *d = NEW_STMTLST(NEW_NUM(4), c);

  JSNODE *found1 = pop_node(d);
  TEST(found1->val == 1);

  JSNODE *found2 = pop_node(d);
  TEST(found2->val == 2);

  JSNODE *found3 = pop_node(d);
  TEST(found3->val == 3);

  JSNODE *found4 = pop_node(d);
  TEST(found4->val == 4);
}

void
test_rewind_node_resets_a_previous_sequence_of_pops()
{
  JSNODE *a = NEW_STMTLST(NEW_NUM(1), NULL);
  JSNODE *b = NEW_STMTLST(NEW_NUM(2), a);
  JSNODE *c = NEW_STMTLST(NEW_NUM(3), b);
  JSNODE *d = NEW_STMTLST(NEW_NUM(4), c);

  JSNODE *found1 = pop_node(d);
  TEST(found1->val == 1);

  JSNODE *found2 = pop_node(d);
  TEST(found2->val == 2);

  JSNODE *found3 = pop_node(d);
  TEST(found3->val == 3);

  JSNODE *found4 = pop_node(d);
  TEST(found4->val == 4);

  // Now rewind, and start over.
  rewind_node(d);

  JSNODE *found5 = pop_node(d);
  TEST(found5->val == 1);

  JSNODE *found6 = pop_node(d);
  TEST(found6->val == 2);

  JSNODE *found7 = pop_node(d);
  TEST(found7->val == 3);

  JSNODE *found8 = pop_node(d);
  TEST(found8->val == 4);
}

void
test_empty_returns_true_on_an_emptied_list()
{
  JSNODE *a = NEW_STMTLST(NEW_NUM(1), NULL);
  JSNODE *b = NEW_STMTLST(NEW_NUM(2), a);
  JSNODE *c = NEW_STMTLST(NEW_NUM(3), b);
  JSNODE *d = NEW_STMTLST(NEW_NUM(4), c);

  TEST(empty_node(d) == false);

  JSNODE *found1 = pop_node(d);
  JSNODE *found2 = pop_node(d);
  JSNODE *found3 = pop_node(d);
  TEST(empty_node(d) == false);
  JSNODE *found4 = pop_node(d);
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
