// test_assignment.js
// ------------------

var assert = console.assert;
var assertEquals = function(a, b) {
  return a === b;
};


// Variable assignment

var x = 0;

x += 4;
assertEquals(4, x);

x /= 2;
assertEquals(2, x);

x -= 1;
assertEquals(1, x);

x *= 7;
assertEquals(7, x);


// Assignment with member expressions

var y = {};

y.a = 42;
assertEquals(42, y.a);

y.b = 56;
y.b += 10;
assertEquals(66, y.b);

y.c = 17;
y.c *= 45;
assertEquals(765, y.c);

// A little crazy, but viable:
var z = {};
z.a = {};
z.a.b = {};
z.a.b.c = {};
z.a.b.c.d = {};
z.a.b.c.d.e = 42;
assertEquals(42, z.a.b.c.d.e);
