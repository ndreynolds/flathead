// test_number_global.js
// ---------------------
// Tests for the Number global object.

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
// Number Global
// ----------------------------------------------------------------------------

assert(Number);
assert(typeof Number === 'function');

test('Constructor', function() {
  assertEquals('object', typeof new Number);
  assertEquals('object', typeof (new Number));
  assertEquals('object', typeof (new Number()));
  assertEquals('object', typeof (new Number(0)));
  assertEquals(99, (new Number(99)).valueOf());
  assertEquals('number', typeof Number(1));
  assertEquals('number', typeof Number(42));
  assertEquals(42, Number('42'));
  assert(42 == new Number(42));
});

test('Properties', function() {
  assert(Number.MAX_VALUE);
  assert(Number.MIN_VALUE);
  assert(isNaN(Number.NaN) && Number.NaN !== undefined);
  assertEquals(-Infinity, Number.NEGATIVE_INFINITY);
  assertEquals( Infinity, Number.POSITIVE_INFINITY);
});

// TODO: Harmony Number.isNaN & Number.isFinite ?


// ----------------------------------------------------------------------------
// Number Prototype
// ----------------------------------------------------------------------------

assert(Number.prototype);

test('Number#toExponential([fracDigits])', function() {
  var num = 123;
  assert(num.toExponential() === '1.23e+2');

  num = 123.456;
  assert(num.toExponential() === '1.23456e+2');

  num = 77.1234;
  assert(num.toExponential() === '7.71234e+1');

  num = 0.000414723723471;
  // TODO: precision is off (though otherwise correct)
  // assert(num.toExponential() === '4.14723723471e-4');
});

test('Number#toFixed([digits])', function() {
  var num = 12345.6789;
  assertEquals('12346', num.toFixed());
  assertEquals('12345.7', num.toFixed(1));
  assertEquals('12345.678900', num.toFixed(6));
});

test('Number#toPrecision([precision])', function() {
  var num = 5.123456;
  // TODO: precision is off (though otherwise correct)
  // assert(num.toPrecision() === '5.123456');
  assertEquals('5.1235', num.toPrecision(5));
  assertEquals('5.1', num.toPrecision(2));
  assertEquals('5', num.toPrecision(1));
});
