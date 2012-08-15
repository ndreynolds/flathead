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

assert(parseInt("42") === 42);
assert(parseInt("3.14") === 3);
assert(isNaN(parseInt("Not a number")));

assert(parseFloat("3.14") === 3.14);
assert(parseFloat("42") === 42);
assert(isNaN(parseFloat("Not a number")));
