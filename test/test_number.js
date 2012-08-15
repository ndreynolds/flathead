// test_number.js
// --------------
// Tests for the Number global object.

var assert = console.assert;

assert(Number.MAX_VALUE);
assert(Number.MIN_VALUE);
assert(isNaN(Number.NaN) && Number.NaN !== undefined);
assert(Number.NEGATIVE_INFINITY === -Infinity);
assert(Number.POSITIVE_INFINITY === Infinity);
