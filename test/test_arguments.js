// test_arguments.js
// -----------------

var assert = console.assert;

// All arguments provided
(function(x, y, z) {
  assert(arguments);
  assert(arguments.length === 3);

  assert(x === 1);
  assert(y === "Joe");
  assert(z['a']);

  assert(arguments[0] === 1);
  assert(arguments[1] === "Joe");
  assert(arguments[2].a === true);

})(1, "Joe", {a: true});


// Missing arguments
(function(x, y, z) {
  assert(arguments);
  assert(arguments.length === 1);

  assert(x === 42);
  assert(y === undefined);
  assert(z === undefined);

  assert(arguments[0] === 42);
})(42);


// Too many arguments
(function(x) {
  assert(arguments);
  assert(arguments.length === 3);

  assert(x === 1);

  assert(arguments[0] === 1);
  assert(arguments[1] === 2);
  assert(arguments[2] === 3);
})(1, 2, 3);


// Parameter named 'arguments' overwrites the arguments object.
(function(arguments) {
  assert(typeof arguments == 'number');
})(42);
