// test_globals.js
// ---------------
// Test methods and properties on the global object.

var assert = console.assert;


// isFinite(value)

assert(isFinite(42));
assert(!isFinite(undefined));
assert(!isFinite(NaN));


// isNaN(value)

assert(isNaN(NaN));
assert(isNaN(undefined));
assert(!isNaN(12));
assert(!isNaN(null));


// undefined

assert(undefined === undefined);


// this

assert(this);


// parseInt(string)
// TODO: handle some of the more exotic cases

// without radix
assert(parseInt("42") === 42);
assert(parseInt("3.14") === 3);
assert(isNaN(parseInt("Not a number")));

// with radix
assert(parseInt(" 0xF", 16) === 15);
assert(parseInt(" F", 16) === 15);
assert(parseInt("17", 8) === 15);
// FIXME: assert(parseInt(021, 8) === 15);
assert(parseInt("015", 10) === 15);
assert(parseInt(15.99, 10) === 15);
// FIXME: assert(parseInt("FXX123", 16) === 15);
assert(parseInt("1111", 2) === 15);
// FIXME: assert(parseInt("15*3", 10) === 15);
// FIXME: assert(parseInt("15e2", 10) === 15);
// FIXME: assert(parseInt("15px", 10) === 15);
assert(parseInt("12", 13) === 15);

assert(isNaN(parseInt("Not a number", 10)));


// parseFloat(string)

assert(parseFloat("3.14") === 3.14);
assert(parseFloat("42") === 42);
assert(isNaN(parseFloat("Not a number")));
