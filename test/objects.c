// objects.c

#include <stdio.h>
#include "jslite.h"

int 
main() 
{
  JLVALUE *a, *b, *c, *d, *e, *f;

  // New name 'int' pointing to new JLValue (integer 42)
  assign("int", new_number(42));
  a = lookup("int")->ptr;
  printf("%f\n", a->number.val);
  printf("%s\n", jl_typeof(a));

  // New name 'bool' pointing to new JLValue (bool true)
  assign("bool", new_boolean(true));
  b = lookup("bool")->ptr;
  printf("%d\n", b->boolean.val);
  printf("%s\n", jl_typeof(b));

  // New name 'string' pointing to new JLValue (string Colonel)
  assign("string", new_string("Colonel"));
  c = lookup("string")->ptr;
  printf("%s\n", c->string.ptr);
  printf("%ld\n", c->string.len);
  printf("%s\n", jl_typeof(c));

  // New name 'float' pointing to new JLValue (number 3.14)
  assign("float", new_number(3.14));
  d = lookup("float")->ptr;
  printf("%f\n", d->number.val);
  printf("%s\n", jl_typeof(d));

  // New name 'copy' pointing to existing JLValue (number 3.14)
  assign("copy", d);
  e = lookup("copy")->ptr;
  printf("%p %p\n", d, e);
  printf("%f\n", e->number.val);
  printf("%s\n", jl_typeof(e));

  // Existing name 'string' pointing to new JLValue (string Foo)
  assign("string", new_string("Foo"));
  f = lookup("string")->ptr;
  printf("%s\n", f->string.ptr);
  printf("%ld\n", f->string.len);
  printf("%s\n", jl_typeof(f));

  return 0;
}
