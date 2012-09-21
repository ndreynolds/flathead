// test_forin.js
// -------------

var assert = console.assert;


var x = 100;
var obj = {a:1, b:2, c:3};
for(var x in obj) {
  assert(x > 0 && x < 4);
}
