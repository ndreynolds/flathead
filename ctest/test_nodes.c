#include "test.h"
#include "../src/jslite.h"
#include "../src/nodes.c"

void
test_pop_node_returns_the_last_node_in_a_linked_list()
{
  JLNode *a = NEW_STMTLST(NEW_NUM(1), NULL);
  JLNode *b = NEW_STMTLST(NEW_NUM(2), a);
  JLNode *c = NEW_STMTLST(NEW_NUM(3), b);
  JLNode *d = NEW_STMTLST(NEW_NUM(4), c);

  JLNode *found1 = pop_node(d);
  TEST(found1->val == 1);

  JLNode *found2 = pop_node(d);
  TEST(found2->val == 2);

  JLNode *found3 = pop_node(d);
  TEST(found3->val == 3);

  JLNode *found4 = pop_node(d);
  TEST(found4->val == 4);
}

void
test_rewind_node_resets_a_previous_sequence_of_pops()
{
  JLNode *a = NEW_STMTLST(NEW_NUM(1), NULL);
  JLNode *b = NEW_STMTLST(NEW_NUM(2), a);
  JLNode *c = NEW_STMTLST(NEW_NUM(3), b);
  JLNode *d = NEW_STMTLST(NEW_NUM(4), c);

  JLNode *found1 = pop_node(d);
  TEST(found1->val == 1);

  JLNode *found2 = pop_node(d);
  TEST(found2->val == 2);

  JLNode *found3 = pop_node(d);
  TEST(found3->val == 3);

  JLNode *found4 = pop_node(d);
  TEST(found4->val == 4);

  // Now rewind, and start over.
  rewind_node(d);

  JLNode *found5 = pop_node(d);
  TEST(found5->val == 1);

  JLNode *found6 = pop_node(d);
  TEST(found6->val == 2);

  JLNode *found7 = pop_node(d);
  TEST(found7->val == 3);

  JLNode *found8 = pop_node(d);
  TEST(found8->val == 4);
}

void
test_empty_returns_true_on_an_emptied_list()
{
  JLNode *a = NEW_STMTLST(NEW_NUM(1), NULL);
  JLNode *b = NEW_STMTLST(NEW_NUM(2), a);
  JLNode *c = NEW_STMTLST(NEW_NUM(3), b);
  JLNode *d = NEW_STMTLST(NEW_NUM(4), c);

  TEST(empty_node(d) == false);

  JLNode *found1 = pop_node(d);
  JLNode *found2 = pop_node(d);
  JLNode *found3 = pop_node(d);
  TEST(empty_node(d) == false);
  JLNode *found4 = pop_node(d);
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
