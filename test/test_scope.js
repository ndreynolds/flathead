// test_scope.js
// -------------

var assert = console.assert;

// ------------------------------------------------------------------
// Function Scope
// ------------------------------------------------------------------

var x = 1;

(function() {
  var y = 2;

  (function() {
    var z = 3;

    (function() {
      // Nested scopes can access outer scope vars
      assert(x == 1);
      assert(y == 2);
      assert(z == 3);
      // Nested scopes can set outer scope vars
      x = 5;
      // TODO: add var decl lifts

      (function() {
        // Variable shadowing
        var y = 9;
      })();
      
    })();

  })();

  // This scope's 'y' is still 2
  assert(y == 2);

})();

// Inner func changed our x
assert(x == 5);
// 'z' was local to 2nd-level func.
assert(this.z === undefined);


// ------------------------------------------------------------------
// Tricks (adequatelygood.com/2010/2/JavaScript-Scoping-and-Hoisting)
// ------------------------------------------------------------------

// foo is hoisted and becomes undefined within the function scope,
// foo then becomes 10 after the assignment.
var foo = 1;
function bar() {
  if (!foo) {
    var foo = 10;
  }
  return foo;
}
assert(bar() === 10);

// a is shadowed within the function scope by the function declaration.
// The inner a (not the outer scope a) is then reassigned to 10.
var a = 1;
function b() {
  a = 10;
  return;
  function a() {}
}
b();
assert(a === 1);
