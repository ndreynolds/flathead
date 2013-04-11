// assertions.js
// -------------

(function() {

  // If this is a Node.js require, we need to set up the assertion helpers on
  // the GLOBAL object for compatability with the other implementations that
  // use `load` to evaluate this file.
  var root = this.GLOBAL || this;

  root.assert = console.assert;

  root.assertTrue = function(a) { assert(a); };

  root.assertFalse = function(a) { assert(!a); };

  root.assertEquals = function(a, b) {
    if (a !== b)
      console.log(a + ' !== ' + b);
    assert(a === b);
  };

  root.assertArrayEquals = function(a, b) {
    assert(a.length === b.length);
    for (var i = 0; i < a.length; i++) {
      assert(a[i] === b[i]);
    }
  };

  root.assertRegExpEquals = function(a, b) {
    assertEquals(a.source, b.source);
    assertEquals(a.global, b.global);
    assertEquals(a.multiline, b.multiline);
    assertEquals(a.ignoreCase, b.ignoreCase);
  };

  root.assertEmptyRegExp = function(r, cmp) {
    // The value of the 'source' prop seems to be implementation-specific
    // when the 'pattern' constructor argument is undefined.
    //
    // Either '(?:)' or '' is acceptable.
    if (!(r.source === '(?:)' || r.source === ''))
      assert(false);
    assertEquals(r.global, cmp.global);
    assertEquals(r.multiline, cmp.multiline);
    assertEquals(r.ignoreCase, cmp.ignoreCase);
  };

  root.assertBetween = function(x, min, max) {
    assert(x > min && x < max); // exclusive
  };

  root.assertClose = function(a, b, tolerance) {
    assert(a < (b + tolerance));
    assert(a > (b - tolerance));
  };

  // For now this is just a way to structure tests, but later it may be used to
  // generate reports.
  root.test = function(name, f) {
    f();
  };

  root.assertionsLoaded = true;
})();
