// test_math.js
// ------------
// Tests for the Math global object, some adapted from v8's mjsunit.

var assert = console.assert;
var assertEquals = function(a, b) { assert(a === b); };

var zero = function() {
  var x = 0.5;
  return (function() { return x - 0.5; })();
};

// Math.abs
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

// Math.floor
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

// Math.sqrt
assertEquals(2, Math.sqrt(4));
assertEquals(0.1, Math.sqrt(0.01));
assert(isNaN(Math.sqrt(NaN)) && Math.sqrt(NaN) !== undefined);
assert(isNaN(Math.sqrt(-1)) && Math.sqrt(-1) !== undefined);
assertEquals(Infinity, Math.sqrt(Infinity));
