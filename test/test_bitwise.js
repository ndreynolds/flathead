// test_bitwise.js
// ---------------

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};

var test = function(name, f) {
  f();
};


test('& Bitwise AND', function() {
  var x = 2600822924;
  assertEquals(0, 1 & 0);
  assertEquals(1, 1 & 1);
  assertEquals(1, -1 & 1);
  assertEquals(8, 14 & 9);
  assertEquals(0, undefined & 1);
  assertEquals(0, 1 & undefined);
  assertEquals(46512102 & x, 46512102 & 2600822924);
  assertEquals(1600822924 & x, 1600822924 & 2600822924);
  assertEquals(2600822924 & x, 2600822924 & 2600822924);
});

test('| Bitwise OR', function() {
  assertEquals(15, 14 | 9);
  assertEquals(1, undefined | 1);
  assertEquals(1, 1 | undefined);
});

test('^ Bitwise XOR', function() {
  assertEquals(7, 14 ^ 9);
  assertEquals(1, undefined ^ 1);
  assertEquals(1, 1 ^ undefined);
});

test('~ Bitwise NOT', function() {
  assertEquals(-10, ~9);
  assertEquals(-1, ~0);
  assertEquals(-2, ~1);
  assertEquals(0, ~-1);
  assertEquals(-101, ~100);
  assertEquals(-1073741825, ~0x40000000);
});

test('<< Bitwise Left Shift', function() {
  assertEquals(36, 9 << 2);
  assertEquals(0, undefined << 1);
  assertEquals(1, 1 << undefined);
});

test('>> Bitwise Right Shift', function() {
  assertEquals(2, 9 >> 2);
  assertEquals(-3, -9 >> 2);
  assertEquals(0, undefined >> 1);
  assertEquals(1, 1 >> undefined);
});

test('>>> Bitwise Right Unsigned Shift', function() {
  assertEquals(2, 9 >>> 2);
  assertEquals(1073741821, -9 >>> 2);
  assertEquals(0, undefined >>> 1);
  assertEquals(1, 1 >>> undefined);
});
