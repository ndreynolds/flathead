// test_helpers.c
// --------------
// Tests for helper functions

#include "test.h"
#include "../src/flathead.h"
#include "../src/flathead.c"

void
test_get_arg_can_retrieve_first_arg()
{
  JSArgs *args = malloc(sizeof(JSArgs*));
  args->arg = JSNUM(42);
  args->next = NULL;

  TEST(fh_get_arg(args, 0)->number.val == 42);
}

void
test_get_arg_handles_empty_case()
{
  JSArgs *args = malloc(sizeof(JSArgs*));
  args->arg = NULL;
  args->next = NULL;

  TEST(fh_get_arg(args, 0)->type == T_NULL);
}

void
test_get_arg_can_retrieve_the_nth_arg()
{
  JSArgs *args0 = malloc(sizeof(JSArgs*));
  JSArgs *args1 = malloc(sizeof(JSArgs*));
  JSArgs *args2 = malloc(sizeof(JSArgs*));

  args0->arg = NULL;
  args0->next = args1;

  args1->arg = NULL;
  args1->next = args2;

  args2->arg = JSNUM(666);
  args2->next = NULL;

  TEST(fh_get_arg(args0, 2)->number.val == 666);
}

void
test_arg_len_returns_the_number_of_arguments()
{
  JSArgs *args0 = malloc(sizeof(JSArgs*));
  JSArgs *args1 = malloc(sizeof(JSArgs*));
  JSArgs *args2 = malloc(sizeof(JSArgs*));

  args0->arg = JSNUM(1);
  args0->next = args1;

  args1->arg = JSNUM(2);
  args1->next = args2;

  args2->arg = JSNUM(3);
  args2->next = NULL;

  TEST(fh_arg_len(args0) == 3);
}

void
test_arg_len_handles_empty_case()
{
  JSArgs *args = malloc(sizeof(JSArgs*));

  args->arg = NULL;
  args->next = NULL;

  TEST(fh_arg_len(args) == 0);
}

int
main()
{
  TEST_START("Helper tests");

  test_get_arg_can_retrieve_first_arg();
  test_get_arg_handles_empty_case();
  test_get_arg_can_retrieve_the_nth_arg();
  test_arg_len_returns_the_number_of_arguments();
  test_arg_len_handles_empty_case();

  TEST_SUMMARY();

  return 0;
}
