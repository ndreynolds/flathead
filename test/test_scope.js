// test_scope.js
// -------------

var assert = console.assert;

// ------------------------------------------------------------------
// FUNCTION SCOPE (easy mode)
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
