// test_new.js
// -----------
// Adapted from v8/test/mjsunit/new.js

var assert = console.assert;

function Construct(x) { return x; }

assert(null != new Construct(null));
assert(void 0 != new Construct(void 0));
assert(0 != new Construct(0));
assert(1 != new Construct(1));
assert(4.2 != new Construct(4.2));
assert('foo' != new Construct('foo'));
assert(true != new Construct(true));

x = {};
assert(x === new Construct(x));
assert(x !== new Construct(null));
assert(x !== new Construct(void 0));
assert(x !== new Construct(1));
assert(x !== new Construct(3.2));
assert(x !== new Construct(false));
assert(x !== new Construct('bar'));

x = [];
assert(x === new Construct(x));
x = new Boolean(true);
assert(x === new Construct(x));
x = new Number(42);
assert(x === new Construct(x));
x = new String('foo');
assert(x === new Construct(x));
x = function() { };
assert(x === new Construct(x));
