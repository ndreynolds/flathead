// test_globals.js
// ---------------
// Test methods and properties on the global object.

var assert = console.assert;

assert(isFinite(42));
assert(!isFinite(undefined));
assert(!isFinite(NaN));

assert(isNaN(NaN));
assert(isNaN(undefined));
assert(!isNaN(12));
assert(!isNaN(null));

assert(undefined === undefined);

assert(this);
