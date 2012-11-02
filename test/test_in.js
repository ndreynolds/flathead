// test_in.js
// ----------
// Tests for the 'in' relational operator

var assert = console.assert;

var test = function(name, f) {
  f();
};


test('in with array', function() {
  var trees = ["redwood", "bay", "cedar", "oak", "maple"];

  assert(0 in trees);
  assert(3 in trees);
  assert(!(6 in trees));
  assert(!('bay' in trees));
  assert('length' in trees);
});

test('in with Math object', function() {
  assert("PI" in Math);
});

test('in with object', function() {
  var mycar = {make: "Honda", model: "Accord", year: 1998};

  assert("make" in mycar);
  assert("model" in mycar);
});
