// test_object.js
// --------------

var assert = console.assert;

// -----------------------------------------------------------------------------
// Prototype
// -----------------------------------------------------------------------------

// Via an object

var x = {a: 42, b: [1,2,3]};
assert(x.toString() === '[object Object]');
assert(x.toLocaleString() === '[object Object]');
assert(x.hasOwnProperty('a'));
console.log(x.hasOwnProperty('a'));
assert(!x.hasOwnProperty('toString'));
assert(x.propertyIsEnumerable('b'));
assert(x.valueOf() === '[object Object]');

// Directly via the prototype

assert(Object.prototype.toString() === '[object Object]');
assert(Object.prototype.toLocaleString() === '[object Object]');
assert(Object.prototype.hasOwnProperty('toString'));
assert(!Object.prototype.hasOwnProperty('a'));
assert(!Object.prototype.propertyIsEnumerable('toString'));
assert(Object.prototype.valueOf() === '[object Object]');

// -----------------------------------------------------------------------------
// Prototype
// -----------------------------------------------------------------------------
