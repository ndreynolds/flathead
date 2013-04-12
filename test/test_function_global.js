// test_function_global.js
// -----------------------
// Tests for the Function global object.

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};

var assertIsFunction = function(test, length) {
  assert(test instanceof Function);
  assertEquals(test.length, length);
};

var test = function(name, f) {
  f();
};


// ----------------------------------------------------------------------------
// Function Global
// ----------------------------------------------------------------------------

assert(Function);
assert(typeof Function === 'function');

test('Constructor', function() {
  assertIsFunction(new Function,                            0);
  assertIsFunction(new Function(),                          0);
  assertIsFunction(new Function('return 42;'),              0);
  assertIsFunction(new Function('a', 'return a;'),          1);
  assertIsFunction(new Function('a', 'b', 'return a + b;'), 2);
  assertIsFunction(new Function('a', 'b', 'c', 'd', '42;'), 4);

  assertEquals(undefined, (new Function)());
  assertEquals(undefined, new Function()());
  assertEquals(42,        new Function('return 42;')());
  assertEquals(42,        new Function('a', 'return a;')(42));
  assertEquals(42,        new Function('a', 'b', 'return a + b;')(21, 21));
});


// ----------------------------------------------------------------------------
// Function Prototype
// ----------------------------------------------------------------------------

assert(Function.prototype);
assertEquals('function', typeof Function.prototype);


x = 42;
var y = 99;
var thisValue = { x: 24, y: 33 };

function add(a, b, c) {
  return a + b + c;
}

function fib(n) {
  return n < 2 ? n : fib(n-1) + fib(n-2);
}

function getX() {
  return this.x;
}

function getY() {
  return y;
}

// Test the this value
function assertThis(test) {
  assert(this === test);
}


test('Function#call(thisValue[, arg1[, arg2[, ...]]])', function() {
  assertEquals(6, add.call(this, 1, 2, 3));
  assertEquals(21, fib.call(this, 8));
  assertEquals(21, fib.call(null, 8));
  assertEquals(21, fib.call(undefined, 8));
  assertEquals(42, getX());
  // Test dynamic this
  assertEquals(24, getX.call(thisValue));
  // Verify that lexical scoping still works when changing dynamic this.
  assertEquals(99, getY.call(thisValue));
  assertThis.call(this, this);
  assertThis.call(thisValue, thisValue);
});

test('Function#apply(thisValue[, argsArray])', function() {
  assertEquals(6, add.apply(this, [1, 2, 3]));
  assertEquals(21, fib.apply(this, [8]));
  assertEquals(21, fib.apply(null, [8]));
  assertEquals(21, fib.apply(undefined, [8]));
  assertEquals(24, getX.apply(thisValue, []));
  assertEquals(99, getY.call(thisValue));
  assertThis.apply(this, [this]);
  assertThis.apply(thisValue, [thisValue]);
});

test('Function#bind(thisValue[, arg1[, arg2[, ...]]])', function() {
  var boundGetX = getX.bind(thisValue);
  var boundGetY = getY.bind(thisValue);

  assertEquals(24, boundGetX());
  assertEquals(99, boundGetY());
  assertThis.bind(thisValue);
});
