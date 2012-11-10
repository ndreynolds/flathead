// test_error_global.js
// --------------------
// Tests for the Error global object.

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
// Error Global
// ----------------------------------------------------------------------------

assert(Error);
assertEquals('function', typeof Error);

test('Constructor', function() {
  var e1 = new Error('stack overflow');
  var e2 = Error();
  var e3 = new Error;
  e3.name = 'BadError';
  e3.message = 'Error code 0x32CC';

  assertEquals('object', typeof new Error);
  assertEquals('object', typeof (new Error));
  assertEquals('object', typeof (new Error()));
  assertEquals('object', typeof (new Error('message')));
  assertEquals('object', typeof Error());
  assertEquals('object', typeof Error('message'));

  assertEquals('Error', e1.name);
  assertEquals('stack overflow', e1.message);
  assertEquals('Error', e2.name);
  assertEquals('', e2.message);
  assertEquals('BadError', e3.name);
  assertEquals('Error code 0x32CC', e3.message);
});


// ----------------------------------------------------------------------------
// Error Prototype
// ----------------------------------------------------------------------------

assert(Error.prototype);

test('Error#toString()', function() {
  var e1 = new Error('stack overflow');
  var e2 = Error();
  var e3 = new Error;
  e3.name = '';
  assertEquals('Error: stack overflow', e1.toString());
  assertEquals('Error', e2.toString());
  assertEquals('', e3.toString());
});
