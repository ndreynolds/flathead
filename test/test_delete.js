// test_delete.js
// --------------

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b) 
    console.log(a + ' !== ' + b);
  assert(a === b);
};


var o = { x: 1 };
assertEquals(true, delete o.x);
assertEquals(undefined, o.x);

var a = 1;
assertEquals(false, delete a);
assertEquals(1, a);

b = 42;
assertEquals(true, delete b);

this.c = 42;
assertEquals(true, delete c);

function d() {}
assertEquals(false, delete d);
assertEquals('function', typeof d);
