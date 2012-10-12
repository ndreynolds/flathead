// test_string_global.js
// ---------------------
// Tests for the String global object.

var assert = console.assert;


// ----------------------------------------------------------------------------
// String Global
// ----------------------------------------------------------------------------

assert(String);


// ----------------------------------------------------------------------------
// String Prototype
// ----------------------------------------------------------------------------

assert(String.prototype);


// String.prototype.charAt(index)

var s = 'abc';
assert(s.charAt() === 'a');
assert(s.charAt(0) === 'a');
assert(s.charAt(1) === 'b');
assert(s.charAt(2) === 'c');
assert(s.charAt(999) === '');
assert(s.charAt(-1) === '');


// String.prototype.concat(string2, string3[, ..., stringN])

s = 'Hello, ';
assert(s.concat('Kevin', ' have a nice day.') === 'Hello, Kevin have a nice day');
assert(s === 'Hello, ');
