// test_functions.js
// -----------------

var assert = console.assert;
var log = console.log;

var assertEquals = function(a, b) {
  return assert(a === b);
};

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

assertEquals(square(3), 9);
assertEquals(add(2, 2), 4);
assertEquals(incrementSquare(9), 82);
assertEquals(incrementSquare(20), 401);
assertEquals(recursive1(1), 10 );
assertEquals(recursive2(1), 10);
