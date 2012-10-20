// test_arrays.js
// --------------

var assert = console.assert;
var assertEquals = function(a, b) {
  assert(a === b);
};


// ----------------------------------------------------------------------------
// Literals & Indexing
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

// Calculated index
var getIndex = function() {
  return 1;
};
var a4 = ['a', 'b', 'c', 'd'];
assertEquals('c', a4[4 - 2]);
assertEquals('b', a4[getIndex()]);


// ----------------------------------------------------------------------------
// Bracket Assignment
// ----------------------------------------------------------------------------

// Easy stuff

var a5 = [];

a5[0] = 10;
a5[1] = 20;
a5[2] = 30;
a5[3] = 40;

assert(a5.length === 4);
assert(a5[0] === 10);
assert(a5[1] === 20);
assert(a5[2] === 30);
assert(a5[3] === 40);

// Indices do not need to be assigned in order (or at all)

var a6 = [];

a6[12] = 42;
a6[10002] = 'hello';

assert(a6.length === 10003);
assert(a6[12] === 42);
assert(a6[10002] === 'hello');
