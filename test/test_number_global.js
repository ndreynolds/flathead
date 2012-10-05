// test_number_global.js
// ---------------------
// Tests for the Number global object.

var assert = console.assert;


// ----------------------------------------------------------------------------
// Number Global
// ----------------------------------------------------------------------------

assert(Number);

assert(Number.MAX_VALUE);
assert(Number.MIN_VALUE);
assert(isNaN(Number.NaN) && Number.NaN !== undefined);
assert(Number.NEGATIVE_INFINITY === -Infinity);
assert(Number.POSITIVE_INFINITY === Infinity);


// ----------------------------------------------------------------------------
// Number Prototype
// ----------------------------------------------------------------------------

assert(Number.prototype);

// Number.prototype.toExponential([fractionDigits])

var num = 77.1234;
assert(num.toExponential() === '7.71234e+1');
