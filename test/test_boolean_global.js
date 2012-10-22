// test_boolean_global.js
// ---------------------
// Tests for the Boolean global object.

var assert = console.assert;


// ----------------------------------------------------------------------------
// Boolean Global
// ----------------------------------------------------------------------------

assert(Boolean);
assert(typeof Boolean === 'function');


// ----------------------------------------------------------------------------
// Boolean Prototype
// ----------------------------------------------------------------------------

assert(Boolean.prototype);

var t = true;
var f = false;


// Boolean.prototype.toString()

assert(t.toString() === 'true');
assert(f.toString() === 'false');


// Boolean.prototype.valueOf()

assert(t.valueOf() === true);
assert(f.valueOf() === false);
