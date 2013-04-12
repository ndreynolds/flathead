// test_switch.js
// --------------

var assert = console.assert;

var test = function(name, f) {
  f();
};

var counter = 0;


test('matching case', function() {
  var x = 42;

  switch (x) {
    case 12:
      assert(false);
      break;
    case 42:
      counter++;
      break;
    default:
      assert(false);
      break;
  }

  assert(counter === 1);
});

test('default case', function() {
  var y = 'not handled';

  switch (y) {
    case 'a':
      assert(false);
      break;
    case 'b':
      assert(false);
      break;
    case 'c':
      assert(false);
      break;
    default:
      counter++;
      break;
  }

  assert(counter === 2);
});

test('fall-through cases and missing breaks', function() {
  var z = 'cat';

  switch (z) {
    case 'cat':
    case 'dog':
      counter++;
    case 'lizard':
      counter++;
  }

  assert(counter === 4);
});

test('cases that follow default are checked first', function() {
  var z = 'cat';

  switch (z) {
    case 'dog':
      assert(false);
      break;
    case 'mouse':
      assert(false);
      break;
    default:
      assert(false);    // This should NOT be executed
      break;
    case 'cat':
      counter++;
  }

  assert(counter === 5);
});
