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

// Syntax allows an empty function body.
var f1 = function() {};

// Statements after a return are not evaluated:
(function() {
  return 12;
  assert();
})();

var add = function(a, b) {
  return a + b;
};

var square = function(x) {
  return x * x;
};

var incrementSquare = function(x) {
  return add(square(x), 1);
};

var recursive1 = function(x) {
  if (x < 10) {
    x = x + 1;
    return arguments.callee(x);
  }
  return x;
};

var recursive2 = function(x) {
  if (x < 10) {
    x = x + 1;
    return recursive2(x);
  }
  return x;
};

assertEquals(9, square(3));
assertEquals(4, add(2, 2));
assertEquals(82, incrementSquare(9));
assertEquals(401, incrementSquare(20));
assertEquals(10, recursive1(1));
assertEquals(10, recursive2(1));
