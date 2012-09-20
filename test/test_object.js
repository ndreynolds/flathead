// test_object.js
// --------------

var assert = console.assert;


// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

var obj = { a: 1, b: 2, c:3 };

// TODO: Object.create()

Object.defineProperty(obj, 'd', { value: 4 });
assert(obj.d === 4);

Object.defineProperties(obj, {
  e: { value: 5 },
  f: { value: 6 }
});
assert(obj.e === 5);
assert(obj.f === 6);

var desc = Object.getOwnPropertyDescriptor(obj, 'a');
assert(desc.value === 1);
assert(desc.configurable);
assert(desc.writable);
assert(desc.enumerable);

var keys = Object.keys(obj);
// TODO: assert(keys.length === 6);
assert(keys[0] === 'a');
assert(keys[1] === 'b');
assert(keys[2] === 'c');
assert(keys[3] === 'd');
assert(keys[4] === 'e');
assert(keys[5] === 'f');


// -----------------------------------------------------------------------------
// Prototype
// -----------------------------------------------------------------------------

// Via an object

var x = {a: 42, b: [1,2,3]};
assert(x.toString() === '[object Object]');
assert(x.toLocaleString() === '[object Object]');
assert(x.hasOwnProperty('a'));
assert(!x.hasOwnProperty('toString'));
assert(x.propertyIsEnumerable('b'));
assert(x.valueOf() === '[object Object]');

// Directly via the prototype

assert(Object.prototype.toString() === '[object Object]');
assert(Object.prototype.toLocaleString() === '[object Object]');
assert(Object.prototype.hasOwnProperty('toString'));
assert(!Object.prototype.hasOwnProperty('a'));
// TODO: assert(!Object.prototype.propertyIsEnumerable('toString'));
assert(Object.prototype.valueOf() === '[object Object]');
