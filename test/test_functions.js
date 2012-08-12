// test_functions.js
// -----------------

assertEquals = function(a, b) {
  return console.assert(a == b);
};

add = function(a, b) {
  return a + b;
};

square = function(x) {
  return x * x;
};

incrementSquare = function(x) {
  return add(square(x), 1);
};

recursive = function(x) {
  if (x < 10) {
    x = x + 1;
    return arguments.callee(x);
  }
  return x;
};

assertEquals(square(3), 9);
assertEquals(add(2, 2), 4);
assertEquals(incrementSquare(9), 82);
assertEquals(incrementSquare(20), 401);
assertEquals(recursive(1), 10);
