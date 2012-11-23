// test_functions.js
// -----------------

var assert = console.assert;
var log = console.log;

var assertEquals = function(a, b) {
  return assert(a === b);
};


// Function declarations can be used pre-defintion

assertEquals(typeof f0, 'function');
assertEquals(f0(), 42);

function f0() {
  return 42;
}


// Named function expressions

var square = function innerSquare(x) {
  assert(square === innerSquare);
  return x * x;
};

assertEquals(144, square(12));
assertEquals('undefined', typeof innerSquare);


// Syntax allows an empty function body.

var f1 = function() {};


// Statements after a return are not evaluated

(function() {
  return 12;
  assert();
})();

var returnTest = function() {
  if (true) return 42;
  return 12;
};

assertEquals(42, returnTest());


// Parameters and return expressions

var add = function(a, b) {
  return a + b;
};

var square = function(x) {
  return x * x;
};

assertEquals(9, square(3));
assertEquals(4, add(2, 2));


// Nested function calls

var incrementSquare = function(x) {
  return add(square(x), 1);
};

assertEquals(82, incrementSquare(9));
assertEquals(401, incrementSquare(20));


// Recursive calls

var recursive1 = function(x) {
  if (x < 10) {
    x = x + 1;
    return arguments.callee(x); // by reference
  }
  return x;
};

assertEquals(10, recursive1(1));

var recursive2 = function(x) {
  if (x < 10) {
    x = x + 1;
    return recursive2(x); // by name
  }
  return x;
};

assertEquals(10, recursive2(1));
