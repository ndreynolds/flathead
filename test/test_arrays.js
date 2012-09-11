// test_arrays.js
// --------------

var assert = console.assert;
var assertEquals = function(a, b) {
  assert(a === b);
};

// ----------------------------------------------------------------------------
// Indexing
// ----------------------------------------------------------------------------

// Simple homogenous array
var a1 = [1, 2, 3];
assertEquals(1, a1[0]);
assertEquals(2, a1[1]);
assertEquals(3, a1[2]);

// Mixed types
var a2 = [1, 'Dog', false, null, undefined, [1, 2, 3], {a: 12}];
assertEquals(1, a2[0]);
assertEquals('Dog', a2[1]);
assertEquals(false, a2[2]);
assertEquals(null, a2[3]);
assertEquals(undefined, a2[4]);
assertEquals(1, a2[5][0]);
assertEquals(2, a2[5][1]);
assertEquals(3, a2[5][2]);
assertEquals(12, a2[6].a);

// Nested arrays
var a3 = [[[[[[1, 2]]]]]];
assertEquals(1, a3[0][0][0][0][0][0]);
assertEquals(2, a3[0][0][0][0][0][1]);
