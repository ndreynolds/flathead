// test_eval.js
// ------------
// Note the unfortunate limitation that until ASI is added, statements within
// strings passed to eval need to be semicolon-terminated.

(this.load || require)((this.load ? 'test' : '.') + '/tools/assertions.js');


test('eval without references', function() {
  assertEquals(1,  eval('1;'));
  assertEquals(4,  eval('2 + 2;'));
  assertEquals(42, eval('(function() { return 42; });')());
});

test('eval with references', function() {
  var x = 42;
  assertEquals(x, eval('x;'));
  assertEquals(f, eval('f;'));
  function f() { return 'The f function'; };
});
