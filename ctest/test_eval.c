// test_eval.c
// -----------
// Tests for evaluation functions

#include "test.h"
#include "../src/flathead.h"
#include "../src/nodes.h"
#include "../src/gc.c"
#include "../src/props.c"
#include "../src/flathead.c"
#include "../src/nodes.c"
#include "../src/eval.c"

void
test_add_handles_numbers()
{
  TEST(fh_add(JSNUM(2), JSNUM(2))->number.val == 4);
  TEST(fh_add(JSNUM(1234.56), JSNUM(789.123))->number.val == 2023.683);
}

void
test_add_concatenates_strings()
{
  TEST(STREQ(fh_add(JSSTR("hello "), JSSTR("world"))->string.ptr, "hello world"));
  TEST(STREQ(fh_add(JSSTR("1"), JSSTR("2"))->string.ptr, "12"));
}

void
test_add_handles_mixed_types()
{
  TEST(STREQ(fh_add(JSSTR("hello"), JSNUM(12))->string.ptr, "hello12"));
  TEST(STREQ(fh_add(JSSTR("hello"), JSUNDEF())->string.ptr, "helloundefined"));
  TEST(fh_add(JSNUM(79), JSNULL())->number.val == 79);
  TEST(fh_add(JSNUM(42), JSUNDEF())->number.is_nan == 1);
  TEST(fh_add(JSNUM(12), JSBOOL(1))->number.val == 13);
  TEST(fh_add(JSNUM(4), JSBOOL(0))->number.val == 4);
  TEST(fh_add(JSNAN(), JSNUM(1))->number.is_nan == 1);
}

void 
test_sub_handles_numbers()
{
  TEST(fh_sub(JSNUM(5), JSNUM(3))->number.val == 2);
  TEST(fh_sub(JSNUM(19822.432), JSNUM(4.230))->number.val == 19818.202);
  TEST(fh_sub(JSNUM(2), JSNUM(5))->number.val == -3);
}

void
test_sub_handles_non_numeric_types()
{
  TEST(fh_sub(JSSTR("hello"), JSSTR("world"))->number.is_nan == 1);
  TEST(fh_sub(JSSTR("hello"), JSUNDEF())->number.is_nan == 1);
  TEST(fh_sub(JSSTR("12"), JSUNDEF())->number.is_nan == 1);
  TEST(fh_sub(JSSTR("5"), JSNULL())->number.val == 5);
  TEST(fh_sub(JSSTR("14"), JSBOOL(1))->number.val == 13);
  TEST(fh_sub(JSSTR("42"), JSSTR("2.4124"))->number.val == 39.5876);
}

void
test_mul_handles_numbers()
{
  TEST(fh_mul(JSNUM(2), JSNUM(2))->number.val == 4);
  TEST(fh_mul(JSNUM(-4), JSNUM(8))->number.val == -32);
  TEST(fh_mul(JSNUM(-457.6412), JSNUM(-2.1409))->number.val == 979.76404508);
}

void
test_mul_handles_non_numeric_types()
{
  TEST(fh_mul(JSSTR("joe"), JSSTR("sixpack"))->number.is_nan == 1);
  TEST(fh_mul(JSSTR("5"), JSSTR("3"))->number.val == 15);
  TEST(fh_mul(JSSTR("1000"), JSNULL())->number.val == 0);
  TEST(fh_mul(JSSTR("4"), JSUNDEF())->number.is_nan == 1);
}

void
test_div_handles_numbers()
{
  TEST(fh_div(JSNUM(9), JSNUM(3))->number.val == 3);
  TEST(fh_div(JSNUM(-4), JSNUM(8))->number.val == -0.5);
  TEST(fh_div(JSNUM(50201513), JSNUM(0.0024))->number.val == 20917297083.333336);
}

void
test_div_handles_non_numeric_types()
{
  TEST(fh_div(JSSTR("joe"), JSSTR("sixpack"))->number.is_nan == 1);
  TEST(fh_div(JSSTR("15"), JSSTR("3"))->number.val == 5);
  TEST(fh_div(JSSTR("40"), JSSTR("-8"))->number.val == -5);
  TEST(fh_div(JSSTR("1000"), JSNULL())->number.is_inf == 1);
  TEST(fh_div(JSSTR("4"), JSUNDEF())->number.is_nan == 1);
}

void
test_mod_handles_numbers()
{
  TEST(fh_mod(JSNUM(9), JSNUM(3))->number.val == 0);
  TEST(fh_mod(JSNUM(-4), JSNUM(3))->number.val == -1);
  TEST(fh_mod(JSNUM(13241), JSNUM(13.3))->number.val == 7.499999999999293);
}

void
test_mod_handles_non_numeric_types()
{
  TEST(fh_mod(JSSTR("joe"), JSSTR("sixpack"))->number.is_nan == 1);
  TEST(fh_mod(JSSTR("5"), JSSTR("3"))->number.val == 2);
  TEST(fh_mod(JSSTR("1000"), JSBOOL(1))->number.val == 0);
  TEST(fh_mod(JSSTR("4"), JSUNDEF())->number.is_nan == 1);
}

void
test_equality()
{
  TEST(fh_eq(JSSTR("Hello"), JSSTR("Hello"), false)->boolean.val == 1);
  TEST(fh_eq(JSNUM(2), JSNUM(2), false)->boolean.val == 1);
  TEST(fh_eq(JSNULL(), JSUNDEF(), false)->boolean.val == 1);
  TEST(fh_eq(JSNULL(), JSNULL(), false)->boolean.val == 1);
  TEST(fh_eq(JSUNDEF(), JSUNDEF(), false)->boolean.val == 1);
  TEST(fh_eq(JSSTR("3.14"), JSNUM(3.14), false)->boolean.val == 1);
  TEST(fh_eq(JSBOOL(1), JSBOOL(1), false)->boolean.val == 1);
  TEST(fh_eq(JSBOOL(1), JSNUM(1), false)->boolean.val == 1);

  JSValue *obj = JSOBJ();
  TEST(fh_eq(obj, obj, false)->boolean.val == 1);

  // NaN still doesn't equal NaN
  TEST(fh_eq(JSNAN(), JSNAN(), false)->boolean.val == 0);
}

void
test_strict_equality()
{
  // Mixed types are never strictly equal.
  TEST(fh_eq(JSNULL(), JSUNDEF(), true)->boolean.val == 0);
  TEST(fh_eq(JSSTR("3.14"), JSNUM(3.14), true)->boolean.val == 0);
  TEST(fh_eq(JSBOOL(1), JSNUM(1), true)->boolean.val == 0);

  TEST(fh_eq(JSBOOL(1), JSBOOL(1), true)->boolean.val == 1);
  TEST(fh_eq(JSNUM(42), JSNUM(42), true)->boolean.val == 1);
  TEST(fh_eq(JSNAN(), JSNAN(), true)->boolean.val == 0);

  JSValue *obj = JSOBJ();
  TEST(fh_eq(obj, obj, true)->boolean.val == 1);
}

void
test_inequality_returns_the_inverse_of_equality()
{
  TEST(fh_eq(JSSTR("Hello"), JSSTR("Hello"), false)->boolean.val == 1);
  TEST(fh_neq(JSSTR("Hello"), JSSTR("Hello"), false)->boolean.val == 0);

  TEST(fh_eq(JSNAN(), JSNAN(), true)->boolean.val == 0);
  TEST(fh_neq(JSNAN(), JSNAN(), true)->boolean.val == 1);
}

void
test_less_than_operator()
{
  TEST(fh_lt(JSNUM(2), JSNUM(4))->boolean.val == 1);
  TEST(fh_lt(JSNUM(1234132), JSNUM(3426423623))->boolean.val == 1);
}

void
test_greater_than_operator()
{
  TEST(fh_gt(JSNUM(4), JSNUM(1))->boolean.val == 1);
  TEST(fh_gt(JSNUM(123532234132), JSNUM(3426423623))->boolean.val == 1);
}

void
test_less_than_or_equal_to_operator()
{
}

void
test_greater_than_or_equal_to_operator()
{
}

int
main()
{
  TEST_START("Evaluation tests");

  test_add_handles_numbers();
  test_add_concatenates_strings();
  test_add_handles_mixed_types();
  test_sub_handles_numbers();
  test_sub_handles_non_numeric_types();
  test_mul_handles_numbers();
  test_mul_handles_non_numeric_types();
  test_div_handles_numbers();
  test_div_handles_non_numeric_types();
  test_mod_handles_numbers();
  test_mod_handles_non_numeric_types();
  test_equality();
  test_strict_equality();
  test_inequality_returns_the_inverse_of_equality();
  test_less_than_or_equal_to_operator();
  test_greater_than_or_equal_to_operator();

  TEST_SUMMARY();

  return 0;
}
