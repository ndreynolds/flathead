// objects.c

#include <stdio.h>
#include "jslite.h"

struct JLVariable *scope = NULL;

struct JLVariable *
lookup(char *name) 
{
  struct JLVariable *var;
  HASH_FIND_STR(scope, name, var);
  return var;
}

void 
assign(char *name, struct JLValue *ptr) 
{
  struct JLVariable *var = lookup(name);
  if (var == NULL) var = malloc(sizeof(struct JLVariable));
  var->name = name;
  var->ptr = ptr;
  HASH_ADD_KEYPTR(hh, scope, var->name, strlen(var->name), var);
}

void
gc() {
  /**
   * Mark and Sweep
   * --------------
   * Go through the hashtable of JLVariables. Follow them to their values.
   * Mark the values 'keepers'. Sweep away everything else. No mercy.
   */
}

char *
jl_typeof(struct JLValue *value) 
{
  /* Per Table 20 of the ECMA5 spec: */
  switch(value->type) 
  {
    case JL_BOOLEAN:
      return "boolean";
    case JL_NUMBER:
      return "number";
    case JL_STRING:
      return "string";
    case JL_ARRAY:
      return "array";
    case JL_NULL:
      return "object";
    default:
      return "undefined";
  }
}

struct JLValue *
new_number(double x) 
{
  struct JLValue *val = malloc(sizeof(struct JLValue));
  val->as.number.value = x;
  val->type = JL_NUMBER;
  return val;
}

struct JLValue *
new_string(char *x) 
{
  struct JLValue *val = malloc(sizeof(struct JLValue));
  val->as.string.ptr = x;
  val->as.string.len = strlen(x);
  val->type = JL_STRING;
  return val;
}

struct JLValue *
new_undef() 
{
  struct JLValue *val = malloc(sizeof(struct JLValue));
  val->type = JL_UNDEF;
  return val;
}

struct JLValue *
new_null() 
{
  struct JLValue *val = malloc(sizeof(struct JLValue));
  val->type = JL_NULL;
  return val;
}

struct JLValue *
new_bool(bool is_true) 
{
  struct JLValue *val = malloc(sizeof(struct JLValue));
  val->as.boolean.value = is_true;
  val->type = JL_BOOLEAN;
  return val;
}

int 
main() 
{
  struct JLValue *a, *b, *c, *d, *e, *f;

  // New name 'int' pointing to new JLValue (integer 42)
  assign("int", new_number(42));
  a = lookup("int")->ptr;
  printf("%f\n", a->as.number.value);
  printf("%s\n", jl_typeof(a));

  // New name 'bool' pointing to new JLValue (bool true)
  assign("bool", new_bool(true));
  b = lookup("bool")->ptr;
  printf("%d\n", b->as.boolean.value);
  printf("%s\n", jl_typeof(b));

  // New name 'string' pointing to new JLValue (string Colonel)
  assign("string", new_string("Colonel"));
  c = lookup("string")->ptr;
  printf("%s\n", c->as.string.ptr);
  printf("%ld\n", c->as.string.len);
  printf("%s\n", jl_typeof(c));

  // New name 'float' pointing to new JLValue (number 3.14)
  assign("float", new_number(3.14));
  d = lookup("float")->ptr;
  printf("%f\n", d->as.number.value);
  printf("%s\n", jl_typeof(d));

  // New name 'copy' pointing to existing JLValue (number 3.14)
  assign("copy", d);
  e = lookup("copy")->ptr;
  printf("%p %p\n", d, e);
  printf("%f\n", e->as.number.value);
  printf("%s\n", jl_typeof(e));

  // Existing name 'string' pointing to new JLValue (string Foo)
  assign("string", new_string("Foo"));
  f = lookup("string")->ptr;
  printf("%s\n", f->as.string.ptr);
  printf("%ld\n", f->as.string.len);
  printf("%s\n", jl_typeof(f));

  return 0;
}
