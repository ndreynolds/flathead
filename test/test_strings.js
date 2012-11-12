// test_strings.js
// ---------------

var assert = console.assert;

var s1 = "Hello World!";
var s2 = 'Hello World!';


// Test character access

assert(s1[-1000] === undefined);
assert(s1[-1] === undefined);
assert(s1[0] === 'H');
assert(s1[1] === 'e');
assert(s1[6] === 'W');
assert(s1[11] === '!');
assert(s1[12] === undefined);
assert(s1[1000] === undefined);

// Direct operations on strings

assert('abc'[1] === 'b');
assert('abc'.toString() === 'abc');
