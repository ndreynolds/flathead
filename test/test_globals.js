// test_globals.js
// ---------------
// Test methods and properties on the global object.
// Make sure everyone's on board. Check built-ins for presence and length.


var assert = console.assert;

var assertNaN = function(x) {
  assert(isNaN(x) && x !== undefined);
};

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};

var assertIsFunction = function(test, length) {
  assert(test instanceof Function);
  assertEquals(test.length, length);
};

var assertIsObject = function(test) {
  assert(test instanceof Object);
};

var test = function(name, f) {
  f();
};


test('global functions', function() {

  test('isFinite(value)', function() {
    assertIsFunction(isFinite, 1);

    assert(isFinite(42));
    assert(!isFinite(undefined));
    assert(!isFinite(NaN));
  });

  test('isNaN(value)', function() {
    assertIsFunction(isNaN, 1);

    assert(isNaN(NaN));
    assert(isNaN(undefined));
    assert(!isNaN(12));
    assert(!isNaN(null));
  });

  test('parseInt(string, radix)', function() {
    assertIsFunction(parseInt, 2);

    // without radix
    assertEquals(42, parseInt("42"));
    assertEquals(3,  parseInt("3.14"));
    assertNaN(parseInt("Not a number"));

    // with radix
    assertEquals(15, parseInt(" 0xF", 16));
    assertEquals(15, parseInt(" F", 16));
    assertEquals(15, parseInt("17", 8));
    // FIXME: assertEquals(15, parseInt(021, 8));
    assertEquals(15, parseInt("015", 10));
    assertEquals(15, parseInt(15.99, 10));
    // FIXME: assertEquals(15, parseInt("FXX123", 16));
    assertEquals(15, parseInt("1111", 2));
    // FIXME: assertEquals(15, parseInt("15*3", 10));
    // FIXME: assertEquals(15, parseInt("15e2", 10));
    // FIXME: assertEquals(15, parseInt("15px", 10));
    assertEquals(15, parseInt("12", 13));
    assertNaN(parseInt("Not a number", 10));
  });

  test('parseFloat(string)', function() {
    assertIsFunction(parseFloat, 1);

    assert(parseFloat("3.14") === 3.14);
    assert(parseFloat("42") === 42);
    assertNaN(parseFloat("Not a number"));
  });

  test('eval(x)', function() {
    // See also: test_eval.js
    assertIsFunction(eval, 1);
    assertEquals(4,  eval('2 + 2;'));
    assertEquals(42, eval('21 * 2;'));
  });

});


test('global values', function() {

  test('undefined', function() {
    assertEquals(undefined, undefined);
  });

  test('this', function() {
    assert(this);
    assertEquals(this, this);
  });

  test('NaN', function() {
    assertNaN(NaN);
  });

  test('Infinity', function() {
    assert(Infinity);
    assert(!isFinite(Infinity));
    assertEquals(Number.POSITIVE_INFINITY, Infinity);
  });

});


test('global constructors', function() {

  test('Array', function() {
    assertIsFunction(Array, 1);

    assertEquals(null, Array.arguments);
    assertEquals(null, Array.caller);
    assertIsObject(Array.prototype);

    assertIsFunction(Array.isArray, 1);

    assertIsFunction(Array.prototype.concat, 1);
    assertIsFunction(Array.prototype.constructor, 1);
    assertIsFunction(Array.prototype.every, 1);
    assertIsFunction(Array.prototype.filter, 1);
    assertIsFunction(Array.prototype.forEach, 1);
    assertIsFunction(Array.prototype.indexOf, 1);
    assertIsFunction(Array.prototype.join, 1);
    assertIsFunction(Array.prototype.lastIndexOf, 1);
    assertIsFunction(Array.prototype.map, 1);
    assertIsFunction(Array.prototype.pop, 0);
    assertIsFunction(Array.prototype.push, 1);
    assertIsFunction(Array.prototype.reduce, 1);
    assertIsFunction(Array.prototype.reduceRight, 1);
    assertIsFunction(Array.prototype.reverse, 0);
    assertIsFunction(Array.prototype.shift, 0);
    assertIsFunction(Array.prototype.slice, 2);
    assertIsFunction(Array.prototype.some, 1);
    assertIsFunction(Array.prototype.sort, 1);
    assertIsFunction(Array.prototype.splice, 2);
    assertIsFunction(Array.prototype.toLocaleString, 0);
    assertIsFunction(Array.prototype.toString, 0);
    assertIsFunction(Array.prototype.unshift, 1);
  });

  test('Boolean', function() {
    assertIsFunction(Boolean, 1);

    assertEquals(null, Boolean.arguments);
    assertEquals(null, Boolean.caller);
    assertIsObject(Boolean.prototype);

    assertIsFunction(Boolean.prototype.constructor, 1);
    assertIsFunction(Boolean.prototype.toString, 0);
    assertIsFunction(Boolean.prototype.valueOf, 0);
  });

  test('Date', function() {
    assertIsFunction(Date, 7);

    assertEquals(null, Date.arguments);
    assertEquals(null, Date.caller);
    assertIsObject(Date.prototype);

    assertIsFunction(Date.UTC, 7);
    assertIsFunction(Date.now, 0);
    assertIsFunction(Date.parse, 1);

    assertIsFunction(Date.prototype.constructor, 7);
    assertIsFunction(Date.prototype.getDate, 0);
    assertIsFunction(Date.prototype.getDay, 0);
    assertIsFunction(Date.prototype.getFullYear, 0);
    assertIsFunction(Date.prototype.getHours, 0);
    assertIsFunction(Date.prototype.getMilliseconds, 0);
    assertIsFunction(Date.prototype.getMinutes, 0);
    assertIsFunction(Date.prototype.getMonth, 0);
    assertIsFunction(Date.prototype.getSeconds, 0);
    assertIsFunction(Date.prototype.getTime, 0);
    assertIsFunction(Date.prototype.getTimezoneOffset, 0);
    assertIsFunction(Date.prototype.getUTCDate, 0);
    assertIsFunction(Date.prototype.getUTCDay, 0);
    assertIsFunction(Date.prototype.getUTCFullYear, 0);
    assertIsFunction(Date.prototype.getUTCHours, 0);
    assertIsFunction(Date.prototype.getUTCMilliseconds, 0);
    assertIsFunction(Date.prototype.getUTCMinutes, 0);
    assertIsFunction(Date.prototype.getUTCMonth, 0);
    assertIsFunction(Date.prototype.getUTCSeconds, 0);
    assertIsFunction(Date.prototype.getYear, 0);
    assertIsFunction(Date.prototype.setDate, 1);
    assertIsFunction(Date.prototype.setFullYear, 3);
    assertIsFunction(Date.prototype.setHours, 4);
    assertIsFunction(Date.prototype.setMilliseconds, 1);
    assertIsFunction(Date.prototype.setMinutes, 3);
    assertIsFunction(Date.prototype.setMonth, 2);
    assertIsFunction(Date.prototype.setSeconds, 2);
    assertIsFunction(Date.prototype.setTime, 1);
    assertIsFunction(Date.prototype.setUTCDate, 1);
    assertIsFunction(Date.prototype.setUTCFullYear, 3);
    assertIsFunction(Date.prototype.setUTCHours, 4);
    assertIsFunction(Date.prototype.setUTCMilliseconds, 1);
    assertIsFunction(Date.prototype.setUTCMinutes, 3);
    assertIsFunction(Date.prototype.setUTCMonth, 2);
    assertIsFunction(Date.prototype.setUTCSeconds, 2);
    assertIsFunction(Date.prototype.setYear, 1);
    assertIsFunction(Date.prototype.toDateString, 0);
    assertIsFunction(Date.prototype.toGMTString, 0);
    assertIsFunction(Date.prototype.toISOString, 0);
    assertIsFunction(Date.prototype.toJSON, 1);
    assertIsFunction(Date.prototype.toLocaleDateString, 0);
    assertIsFunction(Date.prototype.toLocaleString, 0);
    assertIsFunction(Date.prototype.toLocaleTimeString, 0);
    assertIsFunction(Date.prototype.toString, 0);
    assertIsFunction(Date.prototype.toTimeString, 0);
    assertIsFunction(Date.prototype.toUTCString, 0);
    assertIsFunction(Date.prototype.valueOf, 0);
  });

  test('Error', function() {
    assertIsFunction(Error, 1);
    assertIsFunction(EvalError, 1);
    assertIsFunction(RangeError, 1);
    assertIsFunction(ReferenceError, 1);
    assertIsFunction(SyntaxError, 1);
    assertIsFunction(TypeError, 1);
    assertIsFunction(URIError, 1);

    assertEquals(null, Error.arguments);
    assertEquals(null, Error.caller);
    assertIsObject(Error.prototype);

    assertEquals('Error', Error.prototype.name);
    assertEquals('', Error.prototype.message);

    assertIsFunction(Error.prototype.constructor, 1);
    assertIsFunction(Error.prototype.toString, 0);

  });

  test('Function', function() {
    assertIsFunction(Function, 1);

    assertEquals(null, Function.arguments);
    assertEquals(null, Function.caller);
    assertIsObject(Function.prototype);
    assertEquals('function', typeof Function.prototype);

    assertIsFunction(Function.prototype.apply, 2);
    assertIsFunction(Function.prototype.bind, 1);
    assertIsFunction(Function.prototype.call, 1);
    assertIsFunction(Function.prototype.constructor, 1);
    assertIsFunction(Function.prototype.toString, 0);
  });

  test('JSON', function() {
    // TODO
  });

  test('Math', function() {
    assertIsObject(Math);

    assertIsFunction(Math.abs, 1);
    assertIsFunction(Math.acos, 1);
    assertIsFunction(Math.asin, 1);
    assertIsFunction(Math.atan, 1);
    assertIsFunction(Math.atan2, 2);
    assertIsFunction(Math.ceil, 1);
    assertIsFunction(Math.cos, 1);
    assertIsFunction(Math.exp, 1);
    assertIsFunction(Math.floor, 1);
    assertIsFunction(Math.log, 1);
    assertIsFunction(Math.max, 2);
    assertIsFunction(Math.min, 2);
    assertIsFunction(Math.pow, 2);
    assertIsFunction(Math.random, 0);
    assertIsFunction(Math.round, 1);
    assertIsFunction(Math.sin, 1);
    assertIsFunction(Math.sqrt, 1);
    assertIsFunction(Math.tan, 1);
  });

  test('Number', function() {
    assertIsFunction(Number, 1);

    assertEquals(null, Number.arguments);
    assertEquals(null, Number.caller);
    assertIsObject(Number.prototype);

    assertIsFunction(Number.prototype.constructor, 1);
    assertIsFunction(Number.prototype.toExponential, 1);
    assertIsFunction(Number.prototype.toFixed, 1);
    assertIsFunction(Number.prototype.toLocaleString, 0);
    assertIsFunction(Number.prototype.toPrecision, 1);
    assertIsFunction(Number.prototype.toString, 1);
    assertIsFunction(Number.prototype.valueOf, 0);
  });

  test('Object', function() {
    assertIsFunction(Object, 1);

    assertEquals(null, Object.arguments);
    assertEquals(null, Object.caller);
    assertEquals('object', typeof Object.prototype);

    assertIsFunction(Object.create, 2);
    assertIsFunction(Object.defineProperties, 2);
    assertIsFunction(Object.defineProperty, 3);
    assertIsFunction(Object.freeze, 1);
    assertIsFunction(Object.getOwnPropertyDescriptor, 2);
    assertIsFunction(Object.getOwnPropertyNames, 1);
    assertIsFunction(Object.getPrototypeOf, 1);
    assertIsFunction(Object.isExtensible, 1);
    assertIsFunction(Object.isFrozen, 1);
    assertIsFunction(Object.isSealed, 1);
    assertIsFunction(Object.keys, 1);
    assertIsFunction(Object.preventExtensions, 1);
    assertIsFunction(Object.seal, 1);

    assertIsFunction(Object.prototype.constructor, 1);
    assertIsFunction(Object.prototype.hasOwnProperty, 1);
    assertIsFunction(Object.prototype.isPrototypeOf, 1);
    assertIsFunction(Object.prototype.propertyIsEnumerable, 1);
    assertIsFunction(Object.prototype.toLocaleString, 0);
    assertIsFunction(Object.prototype.toString, 0);
    assertIsFunction(Object.prototype.valueOf, 0);
  });

  test('RegExp', function() {
    assertIsFunction(RegExp, 2);

    assertEquals(null, RegExp.arguments);
    assertEquals(null, RegExp.caller);
    assertIsObject(RegExp.prototype);

    assertIsFunction(RegExp.prototype.constructor, 2);
    assertIsFunction(RegExp.prototype.exec, 1);
    assertIsFunction(RegExp.prototype.test, 1);
    assertIsFunction(RegExp.prototype.toString, 0);
  });

  test('String', function() {
    assertIsFunction(String, 1);

    assertEquals(null, String.arguments);
    assertEquals(null, String.caller);
    assertIsObject(String.prototype);

    assertIsFunction(String.fromCharCode, 1);

    assertIsFunction(String.prototype.charAt, 1);
    assertIsFunction(String.prototype.charCodeAt, 1);
    assertIsFunction(String.prototype.concat, 1);
    assertIsFunction(String.prototype.constructor, 1);
    assertIsFunction(String.prototype.indexOf, 1);
    assertIsFunction(String.prototype.lastIndexOf, 1);
    assertIsFunction(String.prototype.localeCompare, 1);
    assertIsFunction(String.prototype.match, 1);
    assertIsFunction(String.prototype.replace, 2);
    assertIsFunction(String.prototype.search, 1);
    assertIsFunction(String.prototype.slice, 2);
    assertIsFunction(String.prototype.split, 2);
    assertIsFunction(String.prototype.substr, 2);
    assertIsFunction(String.prototype.substring, 2);
    assertIsFunction(String.prototype.toLocaleLowerCase, 0);
    assertIsFunction(String.prototype.toLocaleUpperCase, 0);
    assertIsFunction(String.prototype.toLowerCase, 0);
    assertIsFunction(String.prototype.toString, 0);
    assertIsFunction(String.prototype.toUpperCase, 0);
    assertIsFunction(String.prototype.trim, 0);
    assertIsFunction(String.prototype.trimLeft, 0);
    assertIsFunction(String.prototype.trimRight, 0);
    assertIsFunction(String.prototype.valueOf, 0);
  });
});
