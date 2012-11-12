// test_forin.js
// -------------

var assert = console.assert;

// Basic for-in
var x = 100; // expect x to be redeclared
var obj = {a:1, b:2, c:3};
console.log(x);
for(var x in obj) {
  console.log(x);
  assert(obj[x] > 0 && obj[x] < 4);
}
