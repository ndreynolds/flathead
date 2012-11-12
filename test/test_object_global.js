// test_object_global.js
// ---------------------

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};

var test = function(name, f) {
  f();
};


// -----------------------------------------------------------------------------
// Object Global
// -----------------------------------------------------------------------------

assert(Object);
assert(typeof Object === 'function');

test('Constructor', function() {
  assertEquals('object', typeof new Object);
  assertEquals('object', typeof (new Object));
  assertEquals('object', typeof (new Object()));
  assertEquals('object', typeof (new Object(42)));
  assertEquals('object', typeof (new Object('abc')));
  assertEquals('object', typeof (new Object([1, 2, 3])));

  var o1 = new Object('abc');
  var o2 = new Object(42);
  var o3 = new Object(null);

  assertEquals(String.prototype.toString, o1.toString);
  assertEquals(Number.prototype.toString, o2.toString);
  assertEquals(Object.prototype.toString, o3.toString);
});

test('Object.create()', function() {
  var newObj = Object.create(Object.prototype, {
    foo: {writable: true, configurable: true, value: 'hello'},
    bar: {configurable: false, value: 'and'},
    baz: {enumerable: true, value: 'goodbye'}
  });

  assert(newObj.foo === 'hello');
  assert(newObj.bar === 'and');
  assert(newObj.baz === 'goodbye');
});

var obj = { a: 1, b: 2, c: 3 };

test('Object.defineProperty(obj, descriptor)', function() {
  // This one isn't enumerable.
  Object.defineProperty(obj, 'd', { value: 4 });
  assert(obj.d === 4);

  // This one is.
  Object.defineProperty(obj, 'e', { value: 5, enumerable: true });
  assert(obj.e === 5);
});

test('Object.defineProperties(obj, properties)', function() {
  Object.defineProperties(obj, {
    f: { value: 6 },
    g: { value: 7, enumerable: true }
  });
  assert(obj.f === 6);
  assert(obj.g === 7);
});

test('Object.getOwnPropertyDescriptor(obj, prop)', function() {
  var desc = Object.getOwnPropertyDescriptor(obj, 'a');
  assert(desc.value === 1);
  assert(desc.configurable);
  assert(desc.writable);
  assert(desc.enumerable);
});

test('Object.keys(obj)', function() {
  var keys = Object.keys(obj);
  assert(keys.length === 5);
  // Note that we only expect to see enumerable properties.
  assert(keys[0] === 'a');
  assert(keys[1] === 'b');
  assert(keys[2] === 'c');
  assert(keys[3] === 'e');
  assert(keys[4] === 'g');
});


// -----------------------------------------------------------------------------
// Object Prototype
// -----------------------------------------------------------------------------


test('Object#toString()', function() {
  var obj = {a: 42, b: [1,2,3]};
  assertEquals('[object Object]', obj.toString());
  assertEquals('[object Object]', Object.prototype.toString());

  var func = function() {};
  var arr = [1, 2, 3];

  // toString exposes the internal [[Class]] property.
  assertEquals('[object Arguments]', Object.prototype.toString.call(arguments));
  assertEquals('[object Number]', Object.prototype.toString.call(42));
  assertEquals('[object String]', Object.prototype.toString.call('abc'));
  assertEquals('[object Date]', Object.prototype.toString.call(new Date()));
  assertEquals('[object Boolean]', Object.prototype.toString.call(true));
  assertEquals('[object Math]', Object.prototype.toString.call(Math));
  assertEquals('[object RegExp]', Object.prototype.toString.call(/abc/i));
  assertEquals('[object Array]', Object.prototype.toString.call(arr));
  assertEquals('[object Function]', Object.prototype.toString.call(func));
  assertEquals('[object Error]', Object.prototype.toString.call(new Error()));
});

test('Object#toLocaleString()', function() {
  var obj = {a: 42, b: [1,2,3]};
  assert(obj.toLocaleString() === '[object Object]');
  assert(Object.prototype.toLocaleString() === '[object Object]');
});

test('Object#hasOwnProperty(prop)', function() {
  var obj = {a: 42, b: [1,2,3]};
  // TODO
  assert(!obj.hasOwnProperty('toString'));
  assert(obj.hasOwnProperty('a'));
  assert(Object.prototype.hasOwnProperty('toString'));
  assert(!Object.prototype.hasOwnProperty('a'));
});

test('Object#propertyIsEnumerable(prop)', function() {
  var obj = {a: 42, b: [1,2,3]};
  // TODO
  assert(obj.propertyIsEnumerable('b'));
  assert(!Object.prototype.propertyIsEnumerable('toString'));
});

test('Object#valueOf()', function() {
  var obj = {a: 42, b: [1,2,3]};
  // TODO
  assertEquals(obj, obj.valueOf());
  assertEquals(Object.prototype, Object.prototype.valueOf());
});

test('these methods inherit from Function.prototype', function() {
  assertEquals('function', typeof Object.prototype.toString.call);
  assertEquals('function', typeof Object.prototype.hasOwnProperty.apply);
  assertEquals('function', typeof Object.prototype.toLocaleString.bind);
});
