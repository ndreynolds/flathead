// test_in.js
// ----------
// Tests for the 'in' relational operator

var assert = console.assert;

var trees = ["redwood", "bay", "cedar", "oak", "maple"];

assert(0 in trees);
assert(3 in trees);
assert(!(6 in trees));
assert(!('bay' in trees));
assert('length' in trees);

assert("PI" in Math);

var mycar = {make: "Honda", model: "Accord", year: 1998};

assert("make" in mycar);
assert("model" in mycar);
