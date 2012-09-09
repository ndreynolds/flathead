// test_castinc.c
// --------------
// Tests for type coercion

#include "test.h"
#include "../src/flathead.h"
#include "../src/flathead.c"

void
test_numeric_strings_are_correctly_cast()
{
  JSValue *float_str = JSSTR("3.14");
  JSValue *int_str = JSSTR("42");
  
  TEST(JSCAST(float_str, T_NUMBER)->number.val == 3.14);
  TEST(JSCAST(float_str, T_BOOLEAN)->boolean.val == 1);
  TEST(JSCAST(float_str, T_STRING) == float_str);
  TEST(JSCAST(float_str, T_NULL)->type == T_NULL);
  TEST(JSCAST(float_str, T_UNDEF)->type == T_UNDEF);

  TEST(JSCAST(int_str, T_NUMBER)->number.val == 42);
  TEST(JSCAST(int_str, T_BOOLEAN)->boolean.val == 1);
  TEST(JSCAST(int_str, T_STRING) == int_str);
  TEST(JSCAST(int_str, T_NULL)->type == T_NULL);
  TEST(JSCAST(int_str, T_UNDEF)->type == T_UNDEF);
}

void
test_non_numeric_strings_are_NaN_when_cast_to_num()
{
  JSValue *non_num1 = JSSTR("hello");
  JSValue *non_num2 = JSSTR("12a");

  TEST(JSCAST(non_num1, T_NUMBER)->number.is_nan);
  TEST(JSCAST(non_num2, T_NUMBER)->number.is_nan);
}

void
test_numbers_are_correctly_cast()
{
  JSValue *float_num = JSNUM(3.14);
  JSValue *int_num = JSNUM(-568123);

  TEST(JSCAST(float_num, T_NUMBER) == float_num);
  TEST(JSCAST(float_num, T_BOOLEAN)->boolean.val == 1);
  TEST(STREQ(JSCAST(float_num, T_STRING)->string.ptr, "3.14"));
  TEST(JSCAST(float_num, T_NULL)->type == T_NULL);
  TEST(JSCAST(float_num, T_UNDEF)->type == T_UNDEF);

  TEST(JSCAST(int_num, T_NUMBER) == int_num);
  TEST(JSCAST(int_num, T_BOOLEAN)->boolean.val == 1);
  TEST(STREQ(JSCAST(int_num, T_STRING)->string.ptr, "-568123"));
  TEST(JSCAST(int_num, T_NULL)->type == T_NULL);
  TEST(JSCAST(int_num, T_UNDEF)->type == T_UNDEF);
}

void
test_objects_are_correctly_cast()
{
  JSValue *obj = JSOBJ();

  TEST(JSCAST(obj, T_BOOLEAN)->boolean.val == 1);
  TEST(JSCAST(obj, T_NUMBER)->number.is_nan);
  TEST(STREQ(JSCAST(obj, T_STRING)->string.ptr, "[Object object]"));
}

void
test_functions_are_correctly_cast()
{
  void *fake_node = malloc(sizeof(void));
  JSValue *func = JSFUNC(fake_node);

  TEST(JSCAST(func, T_BOOLEAN)->boolean.val == 1);
  TEST(JSCAST(func, T_NUMBER)->number.is_nan);
  TEST(STREQ(JSCAST(func, T_STRING)->string.ptr, "[Function]"));
}

void
test_zero_is_false_others_true()
{
  JSValue *zero = JSNUM(0);
  JSValue *neg = JSNUM(-42);

  TEST(JSCAST(zero, T_BOOLEAN)->boolean.val == 0);
  TEST(JSCAST(neg, T_BOOLEAN)->boolean.val == 1);
}

void
test_keywords_and_specials_cast_to_strings()
{
  TEST(STREQ(JSCAST(JSBOOL(1), T_STRING)->string.ptr, "true"));
  TEST(STREQ(JSCAST(JSBOOL(0), T_STRING)->string.ptr, "false"));
  TEST(STREQ(JSCAST(JSNAN(), T_STRING)->string.ptr, "NaN"));
  TEST(STREQ(JSCAST(JSNULL(), T_STRING)->string.ptr, "null"));
  TEST(STREQ(JSCAST(JSUNDEF(), T_STRING)->string.ptr, "undefined"));
  TEST(STREQ(JSCAST(JSINF(), T_STRING)->string.ptr, "Infinity"));
}

void
test_null_and_undefined_correctly_cast_to_number()
{
  TEST(JSCAST(JSNULL(), T_NUMBER)->number.val == 0);
  TEST(JSCAST(JSUNDEF(), T_NUMBER)->number.is_nan == 1);
}

void
test_true_and_false_correctly_cast_to_number()
{
  TEST(JSCAST(JSBOOL(1), T_NUMBER)->number.val == 1);
  TEST(JSCAST(JSBOOL(0), T_NUMBER)->number.val == 0);
}

int
main()
{
  TEST_START("Casting tests");

  test_numeric_strings_are_correctly_cast();
  test_non_numeric_strings_are_NaN_when_cast_to_num();
  test_numbers_are_correctly_cast();
  test_objects_are_correctly_cast();
  test_functions_are_correctly_cast();
  test_zero_is_false_others_true();
  test_keywords_and_specials_cast_to_strings();
  test_null_and_undefined_correctly_cast_to_number();
  test_true_and_false_correctly_cast_to_number();

  TEST_SUMMARY();

  return 0;
}
