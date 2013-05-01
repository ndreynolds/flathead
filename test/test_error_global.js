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

test('Other Constructors', function() {
  assertEquals('object',         typeof new EvalError);
  assertEquals('EvalError',      (new EvalError).name);
  assertEquals('',               (new EvalError).message);
  assertEquals('test',           (new EvalError('test')).message);

  assertEquals('object',         typeof new RangeError);
  assertEquals('RangeError',     (new RangeError).name);
  assertEquals('',               (new RangeError).message);
  assertEquals('test',           (new RangeError('test')).message);

  console.log('hola');
  assertEquals('object',         typeof new ReferenceError);
  assertEquals('ReferenceError', (new ReferenceError).name);
  assertEquals('',               (new ReferenceError).message);
  assertEquals('test',           (new ReferenceError('test')).message);

  assertEquals('object',         typeof new SyntaxError);
  assertEquals('SyntaxError',    (new SyntaxError).name);
  assertEquals('',               (new SyntaxError).message);
  assertEquals('test',           (new SyntaxError('test')).message);

  assertEquals('object',         typeof new TypeError);
  assertEquals('TypeError',      (new TypeError).name);
  assertEquals('',               (new TypeError).message);
  assertEquals('test',           (new TypeError('test')).message);

  assertEquals('object',         typeof new URIError);
  assertEquals('URIError',       (new URIError).name);
  assertEquals('',               (new URIError).message);
  assertEquals('test',           (new URIError('test')).message);

  assert((new EvalError) instanceof EvalError);
  assert((new EvalError) instanceof Error);
  assert((new EvalError) instanceof Object);

  assert((new RangeError) instanceof RangeError);
  assert((new RangeError) instanceof Error);
  assert((new RangeError) instanceof Object);

  assert((new ReferenceError) instanceof ReferenceError);
  assert((new ReferenceError) instanceof Error);
  assert((new ReferenceError) instanceof Object);

  assert((new SyntaxError) instanceof SyntaxError);
  assert((new SyntaxError) instanceof Error);
  assert((new SyntaxError) instanceof Object);

  assert((new TypeError) instanceof TypeError);
  assert((new TypeError) instanceof Error);
  assert((new TypeError) instanceof Object);

  assert((new URIError) instanceof URIError);
  assert((new URIError) instanceof Error);
  assert((new URIError) instanceof Object);
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
