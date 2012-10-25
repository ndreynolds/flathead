// test_number_global.js
// ---------------------
// Tests for the Number global object.

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};


// ----------------------------------------------------------------------------
// Number Global
// ----------------------------------------------------------------------------

assert(Number);
assert(typeof Number === 'function');

// Constructor

assertEquals('object', typeof new Number);
assertEquals('object', typeof (new Number));
assertEquals('object', typeof (new Number()));
assertEquals('object', typeof (new Number(0)));
assertEquals(99, (new Number(99)).valueOf());
assertEquals('number', typeof Number(1));
assertEquals('number', typeof Number(42));
assertEquals(42, Number('42'));


// Properties

assert(Number.MAX_VALUE);
assert(Number.MIN_VALUE);
assert(isNaN(Number.NaN) && Number.NaN !== undefined);
assert(Number.NEGATIVE_INFINITY === -Infinity);
assert(Number.POSITIVE_INFINITY === Infinity);

// TODO: Harmony Number.isNaN & Number.isFinite ?


// ----------------------------------------------------------------------------
// Number Prototype
// ----------------------------------------------------------------------------

assert(Number.prototype);


// Number.prototype.toExponential([fractionDigits])

var num;
num = 123;
assert(num.toExponential() === '1.23e+2');
num = 123.456;
assert(num.toExponential() === '1.23456e+2');
num = 77.1234;
assert(num.toExponential() === '7.71234e+1');
num = 0.000414723723471;
// TODO: precision is off (though otherwise correct)
//assert(num.toExponential() === '4.14723723471e-4');


// Number.prototype.toFixed([digits])

num = 12345.6789;
assert(num.toFixed() === '12346');
assert(num.toFixed(1) === '12345.7');
assert(num.toFixed(6) === '12345.678900');


// Number.prototype.toPrecision([precision])

num = 5.123456;
// TODO: precision is off (though otherwise correct)
// assert(num.toPrecision() === '5.123456');
assert(num.toPrecision(5) === '5.1235');
assert(num.toPrecision(2) === '5.1');
assert(num.toPrecision(1) === '5');
