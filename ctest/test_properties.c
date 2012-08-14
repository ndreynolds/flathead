#include "test.h"
#include "../src/jslite.h"
#include "../src/jslite.c"

void
test_values_can_be_set_as_a_property_on_an_object()
{
  JLVALUE *val = JLNUM(1);
  JLVALUE *obj = JLOBJ();

  // Assign the value to the object under the property name "my_number"
  jl_set(obj, "my_number", val);

  // Retrieve the property from the object.
  JLPROP *prop = jl_get_prop(obj, "my_number");

  TEST(prop != 0);
  TEST(prop->ptr == val);
  TEST(STREQ(prop->name, "my_number"));

  // Retrieve the value directly
  JLVALUE *found_val = jl_get(obj, "my_number");

  TEST(found_val == val);
}

void
test_multiple_properties_can_be_set_on_an_object()
{
  JLVALUE *val1 = JLBOOL(1);
  JLVALUE *val2 = JLBOOL(0);
  JLVALUE *obj = JLOBJ();

  jl_set(obj, "my_true", val1);
  jl_set(obj, "my_false", val2);
  
  JLVALUE *found1 = jl_get(obj, "my_true");
  JLVALUE *found2 = jl_get(obj, "my_false");

  TEST(found1 == val1);
  TEST(found2 == val2);
}

void
test_looking_up_an_undefined_property_returns_undefined()
{
  JLVALUE *obj = JLOBJ();

  JLPROP *prop = jl_get_prop(obj, "foobar"); 
  JLVALUE *val = jl_get(obj, "foobar");

  TEST(prop == NULL);
  TEST(val->type == T_UNDEF);
}

void
test_property_values_can_be_reassigned_to_an_object()
{
  JLVALUE *some_number = JLNUM(15);
  JLVALUE *some_string = JLSTR("the string");
  JLVALUE *obj = JLOBJ();

  // First assign 15 to obj.x
  jl_set(obj, "x", some_number);
  TEST(jl_get(obj, "x") == some_number);

  // Now assign "the string" to obj.x
  jl_set(obj, "x", some_string);
  TEST(jl_get(obj, "x") == some_string);
}

void
test_circular_references_are_handled()
{
  JLVALUE *obj = JLOBJ();

  jl_set(obj, "this", obj);

  JLVALUE *found = jl_get_rec(obj, "this");

  TEST(found == obj);
}

void
test_recursive_property_gets_retrieve_a_property_from_parent_scope()
{
  JLVALUE *grandparent = JLOBJ();
  JLVALUE *parent = JLOBJ();
  JLVALUE *child = JLOBJ();

  JLVALUE *val = JLSTR("Jack"); 

  parent->object.parent = grandparent;
  child->object.parent = parent;

  jl_set(grandparent, "x", val);
  
  // On non-recursive get fails...
  TEST(jl_get(child, "x")->type == T_UNDEF);
  // But a recursive get retrieves grandparent's 'x'.
  TEST(jl_get_rec(child, "x") == val);
}

void
test_undeclared_assignment_checks_parent_scope()
{
  JLVALUE *some_number = JLNUM(42);
  JLVALUE *another_number = JLNUM(24);
  JLVALUE *parent = JLOBJ();
  JLVALUE *child = JLOBJ();

  // Set up x on the parent object, and link the child to the parent.
  jl_set(parent, "x", some_number);

  child->object.parent = parent;

  // When we recursively set x, it should set the x in the parent scope.
  jl_set_rec(child, "x", another_number);

  TEST(jl_get(parent, "x") == another_number);
  TEST(jl_get(child, "x")->type == T_UNDEF);
  TEST(jl_get_rec(child, "x") == another_number);
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
