#include "test.h"
#include "../src/jslite.h"
#include "../src/jslite.c"

void
test_values_can_be_assigned_to_an_object()
{
  JLVALUE *val = JLNUM(1);
  JLVALUE *obj = JLOBJ();

  // Assign the value to the object under the property name "my_number"
  jl_set(obj, "my_number", val);

  // Retrieve the property from the object.
  JLPROP *prop = jl_lookup(obj, "my_number", 0);

  TEST(prop != 0);
  TEST(prop->ptr == val);
  TEST(STREQ(prop->name, "my_number"));
}

void
test_multiple_values_can_be_stored_on_an_object()
{
  JLVALUE *val1 = JLBOOL(1);
  JLVALUE *val2 = JLBOOL(0);
  JLVALUE *obj = JLOBJ();

  jl_assign(obj, "my_true", val1);
  jl_assign(obj, "my_false", val2);
  
  JLPROP *prop1 = jl_lookup(obj, "my_true", 0);
  JLPROP *prop2 = jl_lookup(obj, "my_false", 0);

  TEST(prop1 != 0);
  TEST(prop2 != 0);
  TEST(prop1->ptr == val1);
  TEST(prop2->ptr == val2);
}

void
test_looking_up_a_undefined_property_returns_a_null_prop_pointer()
{
  JLVALUE *obj = JLOBJ();

  JLPROP *undefined = jl_lookup(obj, "foobar", 0); 

  TEST(undefined == 0);
}

void
test_values_can_be_reassigned_to_an_object()
{
  JLVALUE *some_number = JLNUM(15);
  JLVALUE *some_string = JLSTR("the string");
  JLVALUE *obj;

  // First assign 15 to obj.x
  jl_assign(obj, "x", some_number);
  TEST(jl_lookup(obj, "x", 0)->ptr == some_number);

  // Now assign "the string" to obj.x
  jl_assign(obj, "x", some_string);
  TEST(jl_lookup(obj, "x", 0)->ptr == some_string);
}

int
main()
{
  TEST_START("Property assignment & lookup tests");

  test_values_can_be_assigned_to_an_object();
  test_multiple_values_can_be_stored_on_an_object();
  test_looking_up_a_undefined_property_returns_a_null_prop_pointer();
  test_values_can_be_reassigned_to_an_object();

  TEST_SUMMARY();

  return 0;
}
