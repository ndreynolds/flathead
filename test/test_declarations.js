var assert = console.assert;

// Single variable declaration.
var a = 12;
assert(a === 12);

// Multiple variables.
var b = 42, c = 'foobar', d;
assert(b === 42);
assert(c === 'foobar');
assert(d === undefined);
