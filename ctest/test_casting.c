// test_castinc.c
// --------------
// Tests for type coercion

#include "test.h"
#include "../src/flathead.h"
#include "../src/gc.c"
#include "../src/str.c"
#include "../src/props.c"
#include "../src/flathead.c"

void
test_numeric_strings_are_correctly_cast()
{
  JSValue *float_str = JSSTR("3.14");
  JSValue *int_str = JSSTR("42");
  
  TEST(fh_cast(float_str, T_NUMBER)->number.val == 3.14);
  TEST(fh_cast(float_str, T_BOOLEAN)->boolean.val == 1);
  TEST(fh_cast(float_str, T_STRING) == float_str);
  TEST(fh_cast(float_str, T_NULL)->type == T_NULL);
  TEST(fh_cast(float_str, T_UNDEF)->type == T_UNDEF);

  TEST(fh_cast(int_str, T_NUMBER)->number.val == 42);
  TEST(fh_cast(int_str, T_BOOLEAN)->boolean.val == 1);
  TEST(fh_cast(int_str, T_STRING) == int_str);
  TEST(fh_cast(int_str, T_NULL)->type == T_NULL);
  TEST(fh_cast(int_str, T_UNDEF)->type == T_UNDEF);
}

void
test_non_numeric_strings_are_NaN_when_cast_to_num()
{
  JSValue *non_num1 = JSSTR("hello");
  JSValue *non_num2 = JSSTR("12a");

  TEST(fh_cast(non_num1, T_NUMBER)->number.is_nan);
  TEST(fh_cast(non_num2, T_NUMBER)->number.is_nan);
}

void
test_number_is_correctly_cast()
{
  JSValue *float_num = JSNUM(3.14);
  JSValue *int_num = JSNUM(-568123);

  TEST(fh_cast(float_num, T_NUMBER) == float_num);
  TEST(fh_cast(float_num, T_BOOLEAN)->boolean.val == 1);
  TEST(STREQ(fh_cast(float_num, T_STRING)->string.ptr, "3.14"));
  TEST(fh_cast(float_num, T_NULL)->type == T_NULL);
  TEST(fh_cast(float_num, T_UNDEF)->type == T_UNDEF);

  TEST(fh_cast(int_num, T_NUMBER) == int_num);
  TEST(fh_cast(int_num, T_BOOLEAN)->boolean.val == 1);
  TEST(STREQ(fh_cast(int_num, T_STRING)->string.ptr, "-568123"));
  TEST(fh_cast(int_num, T_NULL)->type == T_NULL);
  TEST(fh_cast(int_num, T_UNDEF)->type == T_UNDEF);

  JSValue *zero = JSNUM(0);
  JSValue *neg = JSNUM(-42);

  TEST(fh_cast(zero, T_BOOLEAN)->boolean.val == 0);
  TEST(fh_cast(neg, T_BOOLEAN)->boolean.val == 1);
}

void
test_object_is_correctly_cast()
{
  JSValue *obj = JSOBJ();
  JSValue *arr = JSOBJ();
  arr->object.is_array = true;

  TEST(fh_cast(obj, T_BOOLEAN)->boolean.val == 1);
  TEST(fh_cast(obj, T_NUMBER)->number.is_nan);
  TEST(STREQ(fh_cast(obj, T_STRING)->string.ptr, "[Object object]"));

  TEST(fh_cast(arr, T_BOOLEAN)->boolean.val == 1);
  TEST(fh_cast(arr, T_NUMBER)->number.val == 0);
  TEST(!fh_cast(arr, T_NUMBER)->number.is_nan);
}

void
test_function_is_correctly_cast()
{
  void *fake_node = malloc(sizeof(void));
  JSValue *func = JSFUNC(fake_node);

  TEST(fh_cast(func, T_BOOLEAN)->boolean.val == 1);
  TEST(fh_cast(func, T_NUMBER)->number.is_nan);
  TEST(STREQ(fh_cast(func, T_STRING)->string.ptr, "[Function]"));
}

void
test_boolean_is_correctly_cast()
{
  TEST(STREQ(fh_cast(JSBOOL(1), T_STRING)->string.ptr, "true"));
  TEST(STREQ(fh_cast(JSBOOL(0), T_STRING)->string.ptr, "false"));

  TEST(fh_cast(JSBOOL(1), T_NUMBER)->number.val == 1);
  TEST(fh_cast(JSBOOL(0), T_NUMBER)->number.val == 0);
}

void
test_infinity_is_correctly_cast()
{
  TEST(STREQ(fh_cast(JSINF(), T_STRING)->string.ptr, "Infinity"));
  TEST(fh_cast(JSINF(), T_BOOLEAN)->boolean.val == 1);

  TEST(STREQ(fh_cast(JSNINF(), T_STRING)->string.ptr, "Infinity"));
  TEST(fh_cast(JSNINF(), T_BOOLEAN)->boolean.val == 1);
}

void
test_nan_is_correctly_cast()
{
  TEST(STREQ(fh_cast(JSNAN(), T_STRING)->string.ptr, "NaN"));
  TEST(fh_cast(JSNAN(), T_BOOLEAN)->boolean.val == 0);
}

void
test_null_is_correctly_cast()
{
  TEST(fh_cast(JSNULL(), T_NUMBER)->number.val == 0);
  TEST(fh_cast(JSNULL(), T_BOOLEAN)->boolean.val == 0);
  TEST(STREQ(fh_cast(JSNULL(), T_STRING)->string.ptr, "null"));
}

void
test_undefined_is_correctly_cast()
{
  TEST(fh_cast(JSUNDEF(), T_NUMBER)->number.is_nan == 1);
  TEST(fh_cast(JSUNDEF(), T_BOOLEAN)->boolean.val == 0);
  TEST(STREQ(fh_cast(JSUNDEF(), T_STRING)->string.ptr, "undefined"));
}

int
main()
{
  TEST_START("Casting tests");

  test_numeric_strings_are_correctly_cast();
  test_non_numeric_strings_are_NaN_when_cast_to_num();
  test_number_is_correctly_cast();
  test_object_is_correctly_cast();
  test_function_is_correctly_cast();
  test_boolean_is_correctly_cast();
  test_infinity_is_correctly_cast();
  test_nan_is_correctly_cast();
  test_null_is_correctly_cast();
  test_undefined_is_correctly_cast();

  TEST_SUMMARY();

  return 0;
}
