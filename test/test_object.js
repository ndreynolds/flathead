// test_object.js
// --------------

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};


// -----------------------------------------------------------------------------
// Literals
// -----------------------------------------------------------------------------

var o = { "a": 0, '---;?"': 1, 'c': 2, d: 3, e: 4, '': 5};

var key = 'a';

assert(o[key] === 0);
assert(o['c'] === 2);
assert(o.c === 2);
assert(o.d === 3);
assert(o['e'] === 4);
assert(o.e === 4);
assert(o['---;?"'] === 1);
assert(o[''] === 5);
