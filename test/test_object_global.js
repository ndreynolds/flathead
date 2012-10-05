// test_object_global.js
// ---------------------

var assert = console.assert;


// -----------------------------------------------------------------------------
// Object Global
// -----------------------------------------------------------------------------

assert(Object);

var obj = { a: 1, b: 2, c:3 };

// Object.create()

// TODO


// Object.defineProperty(obj, descriptor)

// This one isn't enumerable.
Object.defineProperty(obj, 'd', { value: 4 });
assert(obj.d === 4);

// This one is.
Object.defineProperty(obj, 'e', { value: 5, enumerable: true });
assert(obj.e === 5);


// Object.defineProperties(obj, properties)

Object.defineProperties(obj, {
  f: { value: 6 },
  g: { value: 7, enumerable: true }
});
assert(obj.f === 6);
assert(obj.g === 7);


// Object.getOwnPropertyDescriptor(obj, prop)

var desc = Object.getOwnPropertyDescriptor(obj, 'a');
assert(desc.value === 1);
assert(desc.configurable);
assert(desc.writable);
assert(desc.enumerable);


// Object.keys(obj)

var keys = Object.keys(obj);
assert(keys.length === 5);
// Note that we only expect to see enumerable properties.
assert(keys[0] === 'a');
assert(keys[1] === 'b');
assert(keys[2] === 'c');
assert(keys[3] === 'e');
assert(keys[4] === 'g');


// -----------------------------------------------------------------------------
// Object Prototype
// -----------------------------------------------------------------------------

// Via an object

var x = {a: 42, b: [1,2,3]};
assert(x.toString() === '[object Object]');
assert(x.toLocaleString() === '[object Object]');
assert(x.hasOwnProperty('a'));
assert(!x.hasOwnProperty('toString'));
assert(x.propertyIsEnumerable('b'));
assert(x.valueOf() === x);

// Directly via the prototype

assert(Object.prototype.toString() === '[object Object]');
assert(Object.prototype.toLocaleString() === '[object Object]');
assert(Object.prototype.hasOwnProperty('toString'));
assert(!Object.prototype.hasOwnProperty('a'));
assert(!Object.prototype.propertyIsEnumerable('toString'));
assert(Object.prototype.valueOf() === Object.prototype);
