// test_gc.js
// ----------

var assert = console.assert;
var assertEquals = function(a, b) {
  assert(a === b);
};


// Setup some variables that shouldn't be collected.

var f = function() {
  return 'Still here';
};
var x = {a: 12};
var y = 99;
var z = '99 Luftballons';


// Trigger the garbage collector.

// use the method on global, if exposed.
if (typeof gc !== 'undefined') {
  gc();
}
// ...or trigger it the old-fashioned way with a big loop.
else {
  var i = 0;
  while(i < 10000) {
    42;
    i++;
  }
}


// Ensure our variables still exist.

assertEquals('Still here', f());
assertEquals(12, x.a);
assertEquals(99, y);
assertEquals('99 Luftballons', z);
