// test_boolean_global.js
// ---------------------
// Tests for the Boolean global object.

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};

var test = function(name, f) {
  f();
};


// ----------------------------------------------------------------------------
// Boolean Global
// ----------------------------------------------------------------------------

assert(Boolean);
assertEquals('function', typeof Boolean);

test('Constructor', function() {
  assertEquals('object', typeof new Boolean);
  assertEquals('object', typeof (new Boolean));
  assertEquals('object', typeof (new Boolean()));
  assertEquals('object', typeof (new Boolean(true)));
  assertEquals('boolean', typeof Boolean(true));
  assertEquals('boolean', typeof Boolean(false));
  assertEquals(true, (new Boolean(true)).valueOf());
  assertEquals(false, (new Boolean(false)).valueOf());
  assertEquals(false, (new Boolean(false)).valueOf());
  assertEquals(false, Boolean(void 0));
  assertEquals(false, Boolean(null));
  assertEquals(false, Boolean(false));
  assertEquals(true, Boolean(true));
  assertEquals(false, Boolean(0));
  assertEquals(true, Boolean(1));
  assertEquals(true, Boolean(assertEquals));
  assertEquals(true, Boolean(new Object()));
  assert(new Boolean(false) !== false);
  assert(new Boolean(false) == false);
  assert(new Boolean(true) !== true);
  assert(new Boolean(true) == true);
});


// ----------------------------------------------------------------------------
// Boolean Prototype
// ----------------------------------------------------------------------------

assert(Boolean.prototype);

test('Boolean#toString()', function() {
  var t = true, f = false;
  assertEquals('true', t.toString());
  assertEquals('false', f.toString());
  assertEquals('true', true.toString());
  assertEquals('false', false.toString());
});

test('Boolean#valueOf()', function() {
  var t = true, f = false;
  assertEquals(true, t.valueOf());
  assertEquals(false, f.valueOf());
  assertEquals(true, true.valueOf());
  assertEquals(false, false.valueOf());
});
