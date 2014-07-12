// test_math.js
// ------------
// Tests for the Math global object, some adapted from v8's mjsunit.

(this.load || require)((this.load ? 'test' : '.') + '/tools/assertions.js');

var zero = function() {
  var x = 0.5;
  return (function() { return x - 0.5; }());
};


// ----------------------------------------------------------------------------
// Properties
// ----------------------------------------------------------------------------

test('properties', function() {
  // Just sanity checks, see Math.c
  assertClose(2.718,   Math.E,       0.001);
  assertClose(0.693,   Math.LN2,     0.001);
  assertClose(2.303,   Math.LN10,    0.001);
  assertClose(1.443,   Math.LOG2E,   0.001);
  assertClose(0.434,   Math.LOG10E,  0.001);
  assertClose(3.14159, Math.PI,      0.00001);
  assertClose(0.707,   Math.SQRT1_2, 0.001);
  assertClose(1.414,   Math.SQRT2,   0.001);
});

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

test('Math.abs', function() {
  assertEquals(0, Math.abs(0));
  assertEquals(0, Math.abs(zero()));
  assertEquals(0, Math.abs(-0));
  assertEquals(Infinity, Math.abs(Infinity));
  assertEquals(Infinity, Math.abs(-Infinity));
  assert(isNaN(Math.abs(NaN)) && Math.abs(NaN) !== undefined);
  assertEquals(0.1, Math.abs(0.1));
  assertEquals(0.5, Math.abs(0.5));
  assertEquals(0.1, Math.abs(-0.1));
  assertEquals(0.5, Math.abs(-0.5));
  assertEquals(1, Math.abs(1));
  assertEquals(1.5, Math.abs(1.5));
  assertEquals(1, Math.abs(-1));
  assertEquals(1.1, Math.abs(-1.1));
  assertEquals(1.5, Math.abs(-1.5));
  assertEquals(Number.MIN_VALUE, Math.abs(Number.MIN_VALUE));
  assertEquals(Number.MIN_VALUE, Math.abs(-Number.MIN_VALUE));
  assertEquals(Number.MAX_VALUE, Math.abs(Number.MAX_VALUE));
  assertEquals(Number.MAX_VALUE, Math.abs(-Number.MAX_VALUE));
});

test('Math.acos', function() {
  assertEquals(0, Math.acos(1));
  assertEquals(Math.PI / 2, Math.acos(0));
});

test('Math.asin', function() {
  assertEquals(0, Math.asin(0));
  assertEquals(Math.PI / 2, Math.asin(1));
});

test('Math.atan', function() {
  assertEquals(0, Math.atan(0));
  assertEquals(Math.PI / 4, Math.atan(1));
});

test('Math.atan2', function() {
  assertEquals(0, Math.atan2(0, 0));
  assertEquals(Math.PI / 2, Math.atan2(1, 0));
  assertEquals(Math.PI / 4, Math.atan2(1, 1));
});

test('Math.ceil', function() {
  assertEquals(0, Math.ceil(0));
  assertEquals(1, Math.ceil(0.000000000001));
  assertEquals(1, Math.ceil(0.1));
  assertEquals(1, Math.ceil(0.5));
  assertEquals(1, Math.ceil(0.999999999999));
  assertEquals(0, Math.ceil(-0.000000000001));
  assertEquals(-1, Math.ceil(-1.000000000001));
});

test('Math.cos', function() {
  assertEquals(1, Math.cos(0));
  assertEquals(-1, Math.cos(Math.PI));
});

test('Math.exp', function() {
  assertEquals(1, Math.exp(0));
  assertEquals(Math.E, Math.exp(1));
});

test('Math.floor', function() {
  assertEquals(0, Math.floor(0.1));
  assertEquals(0, Math.floor(0.49999999999999994));
  assertEquals(0, Math.floor(0.5));
  assertEquals(0, Math.floor(0.7));
  assertEquals(-1, Math.floor(-0.1));
  assertEquals(-1, Math.floor(-0.5));
  assertEquals(-1, Math.floor(-0.7));
  assertEquals(1, Math.floor(1));
  assertEquals(1, Math.floor(1.1));
  assertEquals(1, Math.floor(1.5));
  assertEquals(1, Math.floor(1.7));
  assertEquals(-1, Math.floor(-1));
  assertEquals(-2, Math.floor(-1.1));
  assertEquals(-2, Math.floor(-1.5));
  assertEquals(-2, Math.floor(-1.7));
  assertEquals(0, Math.floor(Number.MIN_VALUE));
  assertEquals(-1, Math.floor(-Number.MIN_VALUE));
  assertEquals(Number.MAX_VALUE, Math.floor(Number.MAX_VALUE));
  assertEquals(-Number.MAX_VALUE, Math.floor(-Number.MAX_VALUE));
  assertEquals(Infinity, Math.floor(Infinity));
  assertEquals(-Infinity, Math.floor(-Infinity));
});

test('Math.log', function() {
  assertEquals(1, Math.log(Math.E));
  assertEquals(0, Math.log(1));
});

test('Math.max', function() {
  assertEquals(2, Math.max(1, 2));
  assertEquals(2.1, Math.max(2.1, 1.9, 1.1));
  assertEquals(5, Math.max(1, 5, 4, 3, 2));
  assertEquals(-Infinity, Math.max());
  assert(isNaN(Math.max(1, 2, NaN)));
});

test('Math.min', function() {
  assertEquals(1, Math.min(1, 2));
  assertEquals(1.1, Math.min(2.1, 1.9, 1.1));
  assertEquals(0, Math.min(1, 5, 4, 0, 3, 2));
  assertEquals(Infinity, Math.min());
  assert(isNaN(Math.min(1, 2, NaN)));
});

test('Math.pow', function() {
  assertEquals(4, Math.pow(2, 2));
  assertEquals(2, Math.pow(2, 1));
  assertEquals(1, Math.pow(2, 0));
  assertEquals(27, Math.pow(3, 3));
  assertClose(3.482, Math.pow(2, 1.8), 0.001);
});

test('Math.random', function() {
  assert(Math.random() < 1);
  assert(Math.random() >= 0);
});

test('Math.round', function() {
  assertEquals(1, Math.round(0.5));
  assertEquals(1, Math.round(0.7));
  assertEquals(1, Math.round(1));
  assertEquals(1, Math.round(1.1));
  assertEquals(1, Math.round(1.49999));
  assertEquals(-1, Math.round(-0.7));
  assertEquals(-1, Math.round(-1.5));
  assertEquals(-0, Math.round(-0.5));
});

test('Math.sin', function() {
  assertEquals(0, Math.sin(0));
  assertEquals(1, Math.sin(Math.PI / 2));
});

test('Math.sqrt', function() {
  assertEquals(2, Math.sqrt(4));
  assertEquals(0.1, Math.sqrt(0.01));
  assert(isNaN(Math.sqrt(NaN)) && Math.sqrt(NaN) !== undefined);
  assert(isNaN(Math.sqrt(-1)) && Math.sqrt(-1) !== undefined);
  assertEquals(Infinity, Math.sqrt(Infinity));
});

test('Math.tan', function() {
  assertEquals(0, Math.tan(0));
});
