#include "test.h"
#include "../src/jslite.h"
#include "../src/jslite.c"
#include "../src/eval.c"

void
test_add_handles_numbers()
{
  TEST(jl_add(JLNUM(2), JLNUM(2))->number.val == 4);
  TEST(jl_add(JLNUM(1234.56), JLNUM(789.123))->number.val == 2023.683);
}

void
test_add_concatenates_strings()
{
  TEST(STREQ(jl_add(JLSTR("hello "), JLSTR("world"))->string.ptr, "hello world"));
  TEST(STREQ(jl_add(JLSTR("1"), JLSTR("2"))->string.ptr, "12"));
}

void
test_add_handles_mixed_types()
{
  TEST(STREQ(jl_add(JLSTR("hello"), JLNUM(12))->string.ptr, "hello12"));
  TEST(STREQ(jl_add(JLSTR("hello"), JLUNDEF())->string.ptr, "helloundefined"));
  TEST(jl_add(JLNUM(79), JLNULL())->number.val == 79);
  TEST(jl_add(JLNUM(42), JLUNDEF())->number.is_nan == 1);
  TEST(jl_add(JLNUM(12), JLBOOL(1))->number.val == 13);
  TEST(jl_add(JLNUM(4), JLBOOL(0))->number.val == 4);
}

void 
test_sub_handles_numbers()
{
  TEST(jl_sub(JLNUM(5), JLNUM(3))->number.val == 2);
  TEST(jl_sub(JLNUM(19822.432), JLNUM(4.230))->number.val == 19818.202);
  TEST(jl_sub(JLNUM(2), JLNUM(5))->number.val == -3);
}

void
test_sub_handles_non_numeric_types()
{
  TEST(jl_sub(JLSTR("hello"), JLSTR("world"))->number.is_nan == 1);
  TEST(jl_sub(JLSTR("hello"), JLUNDEF())->number.is_nan == 1);
  TEST(jl_sub(JLSTR("12"), JLUNDEF())->number.is_nan == 1);
  TEST(jl_sub(JLSTR("5"), JLNULL())->number.val == 5);
  TEST(jl_sub(JLSTR("14"), JLBOOL(1))->number.val == 13);
  TEST(jl_sub(JLSTR("42"), JLSTR("2.4124"))->number.val == 39.5876);
}

void
test_mul_handles_numbers()
{
  TEST(jl_mul(JLNUM(2), JLNUM(2))->number.val == 4);
  TEST(jl_mul(JLNUM(-4), JLNUM(8))->number.val == -32);
  TEST(jl_mul(JLNUM(-457.6412), JLNUM(-2.1409))->number.val == 979.76404508);
}

void
test_mul_handles_non_numeric_types()
{
  TEST(jl_mul(JLSTR("joe"), JLSTR("sixpack"))->number.is_nan == 1);
  TEST(jl_mul(JLSTR("5"), JLSTR("3"))->number.val == 15);
  TEST(jl_mul(JLSTR("1000"), JLNULL())->number.val == 0);
  TEST(jl_mul(JLSTR("4"), JLUNDEF())->number.is_nan == 1);
}

void
test_div_handles_numbers()
{
  TEST(jl_div(JLNUM(9), JLNUM(3))->number.val == 3);
  TEST(jl_div(JLNUM(-4), JLNUM(8))->number.val == -0.5);
  TEST(jl_div(JLNUM(50201513), JLNUM(0.0024))->number.val == 20917297083.333336);
}

void
test_div_handles_non_numeric_types()
{
  TEST(jl_div(JLSTR("joe"), JLSTR("sixpack"))->number.is_nan == 1);
  TEST(jl_div(JLSTR("15"), JLSTR("3"))->number.val == 5);
  TEST(jl_div(JLSTR("40"), JLSTR("-8"))->number.val == -5);
  TEST(jl_div(JLSTR("1000"), JLNULL())->number.is_inf == 1);
  TEST(jl_div(JLSTR("4"), JLUNDEF())->number.is_nan == 1);
}

void
test_mod_handles_numbers()
{
  TEST(jl_mod(JLNUM(9), JLNUM(3))->number.val == 0);
  TEST(jl_mod(JLNUM(-4), JLNUM(3))->number.val == -1);
  TEST(jl_mod(JLNUM(13241), JLNUM(13.3))->number.val == 7.499999999999293);
}

void
test_mod_handles_non_numeric_types()
{
  TEST(jl_mod(JLSTR("joe"), JLSTR("sixpack"))->number.is_nan == 1);
  TEST(jl_mod(JLSTR("5"), JLSTR("3"))->number.val == 2);
  TEST(jl_mod(JLSTR("1000"), JLBOOL(1))->number.val == 0);
  TEST(jl_mod(JLSTR("4"), JLUNDEF())->number.is_nan == 1);
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

  TEST_SUMMARY();

  return 0;
}
