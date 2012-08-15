// test_properties.c
// -----------------
// Tests for property sets and gets.

#include "test.h"
#include "../src/flathead.h"
#include "../src/flathead.c"

void
test_values_can_be_set_as_a_property_on_an_object()
{
  JSVALUE *val = JSNUM(1);
  JSVALUE *obj = JSOBJ();

  // Assign the value to the object under the property name "my_number"
  fh_set(obj, "my_number", val);

  // Retrieve the property from the object.
  JSPROP *prop = fh_get_prop(obj, "my_number");

  TEST(prop != 0);
  TEST(prop->ptr == val);
  TEST(STREQ(prop->name, "my_number"));

  // Retrieve the value directly
  JSVALUE *found_val = fh_get(obj, "my_number");

  TEST(found_val == val);
}

void
test_multiple_properties_can_be_set_on_an_object()
{
  JSVALUE *val1 = JSBOOL(1);
  JSVALUE *val2 = JSBOOL(0);
  JSVALUE *obj = JSOBJ();

  fh_set(obj, "my_true", val1);
  fh_set(obj, "my_false", val2);
  
  JSVALUE *found1 = fh_get(obj, "my_true");
  JSVALUE *found2 = fh_get(obj, "my_false");

  TEST(found1 == val1);
  TEST(found2 == val2);
}

void
test_looking_up_an_undefined_property_returns_undefined()
{
  JSVALUE *obj = JSOBJ();

  JSPROP *prop = fh_get_prop(obj, "foobar"); 
  JSVALUE *val = fh_get(obj, "foobar");

  TEST(prop == NULL);
  TEST(val->type == T_UNDEF);
}

void
test_property_values_can_be_reassigned_to_an_object()
{
  JSVALUE *some_number = JSNUM(15);
  JSVALUE *some_string = JSSTR("the string");
  JSVALUE *obj = JSOBJ();

  // First assign 15 to obj.x
  fh_set(obj, "x", some_number);
  TEST(fh_get(obj, "x") == some_number);

  // Now assign "the string" to obj.x
  fh_set(obj, "x", some_string);
  TEST(fh_get(obj, "x") == some_string);
}

void
test_circular_references_are_handled()
{
  JSVALUE *obj = JSOBJ();

  fh_set(obj, "this", obj);

  JSVALUE *found = fh_get_rec(obj, "this");

  TEST(found == obj);
}

void
test_recursive_property_gets_retrieve_a_property_from_parent_scope()
{
  JSVALUE *grandparent = JSOBJ();
  JSVALUE *parent = JSOBJ();
  JSVALUE *child = JSOBJ();

  JSVALUE *val = JSSTR("Jack"); 

  parent->object.parent = grandparent;
  child->object.parent = parent;

  fh_set(grandparent, "x", val);
  
  // On non-recursive get fails...
  TEST(fh_get(child, "x")->type == T_UNDEF);
  // But a recursive get retrieves grandparent's 'x'.
  TEST(fh_get_rec(child, "x") == val);
}

void
test_undeclared_assignment_checks_parent_scope()
{
  JSVALUE *some_number = JSNUM(42);
  JSVALUE *another_number = JSNUM(24);
  JSVALUE *parent = JSOBJ();
  JSVALUE *child = JSOBJ();

  // Set up x on the parent object, and link the child to the parent.
  fh_set(parent, "x", some_number);

  child->object.parent = parent;

  // When we recursively set x, it should set the x in the parent scope.
  fh_set_rec(child, "x", another_number);

  TEST(fh_get(parent, "x") == another_number);
  TEST(fh_get(child, "x")->type == T_UNDEF);
  TEST(fh_get_rec(child, "x") == another_number);
}

int
main()
{
  TEST_START("Property getting & setting tests");

  test_values_can_be_set_as_a_property_on_an_object();
  test_multiple_properties_can_be_set_on_an_object();
  test_looking_up_an_undefined_property_returns_undefined();
  test_property_values_can_be_reassigned_to_an_object();
  test_circular_references_are_handled();
  test_recursive_property_gets_retrieve_a_property_from_parent_scope();
  test_undeclared_assignment_checks_parent_scope();

  TEST_SUMMARY();

  return 0;
}
