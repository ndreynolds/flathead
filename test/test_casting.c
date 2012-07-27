#include "test.h"
#include "../src/jslite.h"
#include "../src/objects.c"

void
test_numeric_strings_are_correctly_cast()
{
  JLVALUE *float_str = JLSTR("3.14");
  JLVALUE *int_str = JLSTR("42");
  
  TEST(JLCAST(float_str, T_NUMBER)->number.val == 3.14);
  TEST(JLCAST(float_str, T_BOOLEAN)->boolean.val == 1);
  TEST(JLCAST(float_str, T_STRING) == float_str);
  TEST(JLCAST(float_str, T_NULL)->type == T_NULL);
  TEST(JLCAST(float_str, T_UNDEF)->type == T_UNDEF);

  TEST(JLCAST(int_str, T_NUMBER)->number.val == 42);
  TEST(JLCAST(int_str, T_BOOLEAN)->boolean.val == 1);
  TEST(JLCAST(int_str, T_STRING) == int_str);
  TEST(JLCAST(int_str, T_NULL)->type == T_NULL);
  TEST(JLCAST(int_str, T_UNDEF)->type == T_UNDEF);
}

void
test_non_numeric_strings_are_NaN_when_cast_to_num()
{
  JLVALUE *non_num1 = JLSTR("hello");
  JLVALUE *non_num2 = JLSTR("12a");

  TEST(JLCAST(non_num1, T_NUMBER)->number.is_nan);
  TEST(JLCAST(non_num2, T_NUMBER)->number.is_nan);
}

void
test_numbers_are_correctly_cast()
{
  JLVALUE *float_num = JLNUM(3.14);
  JLVALUE *int_num = JLNUM(-568123);

  TEST(JLCAST(float_num, T_NUMBER) == float_num);
  TEST(JLCAST(float_num, T_BOOLEAN)->boolean.val == 1);
  TEST(STREQ(JLCAST(float_num, T_STRING)->string.ptr, "3.14"));
  TEST(JLCAST(float_num, T_NULL)->type == T_NULL);
  TEST(JLCAST(float_num, T_UNDEF)->type == T_UNDEF);

  TEST(JLCAST(int_num, T_NUMBER) == int_num);
  TEST(JLCAST(int_num, T_BOOLEAN)->boolean.val == 1);
  TEST(STREQ(JLCAST(int_num, T_STRING)->string.ptr, "-568123"));
  TEST(JLCAST(int_num, T_NULL)->type == T_NULL);
  TEST(JLCAST(int_num, T_UNDEF)->type == T_UNDEF);
}

void
test_zero_is_false_others_true()
{
  JLVALUE *zero = JLNUM(0);
  JLVALUE *neg = JLNUM(-42);

  TEST(JLCAST(zero, T_BOOLEAN)->boolean.val == 0);
  TEST(JLCAST(neg, T_BOOLEAN)->boolean.val == 1);
}

void
test_keywords_and_specials_cast_to_strings()
{
  TEST(STREQ(JLCAST(JLBOOL(1), T_STRING)->string.ptr, "true"));
  TEST(STREQ(JLCAST(JLBOOL(0), T_STRING)->string.ptr, "false"));
  TEST(STREQ(JLCAST(JLNAN(), T_STRING)->string.ptr, "NaN"));
  TEST(STREQ(JLCAST(JLNULL(), T_STRING)->string.ptr, "null"));
  TEST(STREQ(JLCAST(JLUNDEF(), T_STRING)->string.ptr, "undefined"));
  TEST(STREQ(JLCAST(JLINF(), T_STRING)->string.ptr, "Infinity"));
}

void
test_null_and_undefined_correctly_cast_to_number()
{
  TEST(JLCAST(JLNULL(), T_NUMBER)->number.val == 0);
  TEST(JLCAST(JLUNDEF(), T_NUMBER)->number.is_nan == 1);
}

void
test_true_and_false_correctly_cast_to_number()
{
  TEST(JLCAST(JLBOOL(1), T_NUMBER)->number.val == 1);
  TEST(JLCAST(JLBOOL(0), T_NUMBER)->number.val == 0);
}

int
main()
{
  TEST_START("Casting tests");

  test_numeric_strings_are_correctly_cast();
  test_non_numeric_strings_are_NaN_when_cast_to_num();
  test_numbers_are_correctly_cast();
  test_zero_is_false_others_true();
  test_keywords_and_specials_cast_to_strings();
  test_null_and_undefined_correctly_cast_to_number();
  test_true_and_false_correctly_cast_to_number();

  TEST_SUMMARY(tests);

  return 0;
}
