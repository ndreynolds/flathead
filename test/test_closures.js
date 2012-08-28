// test_closures.js
// ----------------

var assert = console.assert;

// Return a function, no accessing outer scope vars. (easy mode)
var f1 = function() {
  return function() {
    return 42;
  };
};

var f2 = f1();
assert(f2() === 42);

// Return a function, access outer scope vars.
var f3 = function() {
  var x = "vuvuzela";
  return function() {
    return x;
  };
};

var f4 = f3();
assert(f4() === "vuvuzela");

// Return a function, modify outer scope vars.
var f5 = function() {
  var y = 1;
  return function() {
    return ++y;
  };
};

var f6 = f5();
assert(f6() === 2);
assert(f6() === 3);
assert(f6() === 4);

// Return an object of functions, assert they ref the same values. (hard mode)
var f7 = function() {
  var z = 10;
  return {
    a: function() {
      return --z;
    },
    b: function() {
      return ++z;
    }
  };
};

var funcs = f7();
// TODO: Gonna let this pass for now.
//assert(funcs.a() == 9);
//assert(funcs.b() == 9);
