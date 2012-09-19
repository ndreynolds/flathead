// test_properties.c
// -----------------
// Tests for property sets and gets.

#include "test.h"
#include "../src/flathead.h"
#include "../src/gc.c"
#include "../src/flathead.c"

void
test_values_can_be_set_as_a_property_on_an_object()
{
  JSValue *val = JSNUM(1);
  JSValue *obj = JSOBJ();

  // Assign the value to the object under the property name "my_number"
  fh_set(obj, "my_number", val);

  // Retrieve the property from the object.
  JSProp *prop = fh_get_prop(obj, "my_number");

  TEST(prop != 0);
  TEST(prop->ptr == val);
  TEST(STREQ(prop->name, "my_number"));

  // Retrieve the value directly
  JSValue *found_val = fh_get(obj, "my_number");

  TEST(found_val == val);
}

void
test_multiple_properties_can_be_set_on_an_object()
{
  JSValue *val1 = JSBOOL(1);
  JSValue *val2 = JSBOOL(0);
  JSValue *obj = JSOBJ();

  fh_set(obj, "my_true", val1);
  fh_set(obj, "my_false", val2);
  
  JSValue *found1 = fh_get(obj, "my_true");
  JSValue *found2 = fh_get(obj, "my_false");

  TEST(found1 == val1);
  TEST(found2 == val2);
}

void
test_looking_up_an_undefined_property_returns_undefined()
{
  JSValue *obj = JSOBJ();

  JSProp *prop = fh_get_prop(obj, "foobar"); 
  JSValue *val = fh_get(obj, "foobar");

  TEST(prop == NULL);
  TEST(val->type == T_UNDEF);
}

void
test_property_values_can_be_reassigned_to_an_object()
{
  JSValue *some_number = JSNUM(15);
  JSValue *some_string = JSSTR("the string");
  JSValue *obj = JSOBJ();

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
  JSValue *obj = JSOBJ();

  fh_set(obj, "this", obj);

  JSValue *found = fh_get_rec(obj, "this");

  TEST(found == obj);
}

void
test_recursive_property_gets_retrieve_a_property_from_parent_scope()
{
  JSValue *grandparent = JSOBJ();
  JSValue *parent = JSOBJ();
  JSValue *child = JSOBJ();

  JSValue *val = JSSTR("Jack"); 

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
  JSValue *some_number = JSNUM(42);
  JSValue *another_number = JSNUM(24);
  JSValue *parent = JSOBJ();
  JSValue *child = JSOBJ();

  // Set up x on the parent object, and link the child to the parent.
  fh_set(parent, "x", some_number);

  child->object.parent = parent;

  // When we recursively set x, it should set the x in the parent scope.
  fh_set_rec(child, "x", another_number);

  TEST(fh_get(parent, "x") == another_number);
  TEST(fh_get(child, "x")->type == T_UNDEF);
  TEST(fh_get_rec(child, "x") == another_number);
}

void
test_set_rec_works_repeatedly()
{
  JSValue *first = JSNUM(1);
  JSValue *second = JSNUM(2);
  JSValue *third = JSNUM(3);

  JSValue *parent = JSOBJ();
  JSValue *child = JSOBJ();
  child->object.parent = parent;

  fh_set(parent, "x", first);

  TEST(fh_get(parent, "x") == first);
  TEST(fh_get(child, "x")->type == T_UNDEF);
  TEST(fh_get_rec(child, "x") == first);

  fh_set_rec(child, "x", second);

  TEST(fh_get(parent, "x") == second);
  TEST(fh_get(child, "x")->type == T_UNDEF);
  TEST(fh_get_rec(child, "x") == second);

  fh_set_rec(child, "x", third);

  TEST(fh_get(parent, "x") == third);
  TEST(fh_get(child, "x")->type == T_UNDEF);
  TEST(fh_get_rec(child, "x") == third);
}

void test_set_rec_works_on_local_scope()
{
  JSValue *val = JSNUM(42);
  JSValue *scope = JSOBJ();

  fh_set_rec(scope, "x", val);

  TEST(fh_get(scope, "x") == val);
  TEST(fh_get_rec(scope, "x") == val);
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
  test_set_rec_works_repeatedly();
  test_set_rec_works_on_local_scope();

  TEST_SUMMARY();

  return 0;
}
