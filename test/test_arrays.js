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
//assertEquals('c', a4[4 - 2]);
//assertEquals('b', a4[getIndex()]);


// ----------------------------------------------------------------------------
// Array Global
// ----------------------------------------------------------------------------

assert(Array);

assert(Array.isArray([]));
assert(Array.isArray([1]));
// TODO: (waiting on new operator) assert(Array.isArray( new Array() );
assert(Array.isArray(Array.prototype));

assert(!Array.isArray());
assert(!Array.isArray({}));
assert(!Array.isArray(null));
assert(!Array.isArray(undefined));
assert(!Array.isArray(17));
assert(!Array.isArray("Array"));
assert(!Array.isArray(true));
assert(!Array.isArray(false));


// ----------------------------------------------------------------------------
// Array Prototype
// ----------------------------------------------------------------------------

var a5 = [1, 2, 3, 4];

// Array.prototype.pop()
assert(a5.pop() === 4);
assert(a5.length === 3);
assert(a5.pop() === 3);
assert(a5.length === 2);

// Array.prototype.push(element1, ..., elementN)
assert(a5.push('lions') === 3);
assert(a5.length === 3);
assert(a5.push('tigers', 'and', 'bears') === 6);
assert(a5.length === 6);
