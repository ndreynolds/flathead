// test_forin.js
// -------------

var assert = console.assert;

// Basic for-in
var x = 100; // expect x to be redeclared
var obj = {a:1, b:2, c:3};
for(var x in obj) {
  assert(x > 0 && x < 4);
}
