// test_forin.js
// -------------

var assert = console.assert;

var assertBetween = function(x, min, max) {
  assert(x > min && x < max); // exclusive
};

var assertEquals = function(a, b) {
  assert(a === b);
};

var test = function(name, f) {
  f();
};


test('for-in with variable declaration on object', function() {
  var obj = { a: 1, b: 2, c: 3 };

  for (var x in obj) {
    assert(obj[x] > 0 && obj[x] < 4);
  }
  assertEquals('c', x);
});

test('for-in with variable declaration on array', function() {
  var arr = [1, 2, 3, 4, 5, 6, 7, 8, 9];
 
  for (var i in arr) {
    assertBetween(arr[i], 0, 10);
  }
  assertEquals('8', i);
});

test('for-in with lhs expression on object', function() {
  var obj = { a: 1, b: 2, c: 3 };

  var k;
  for (k in obj) {
    assertBetween(obj[k], 0, 4);
  }
  assertEquals('c', k);
});

test('for-in with lhs member expression on object', function() {
  var obj = { a: 1, b: 2, c: 3 };
  var x = { key: null };

  for (x.key in obj) {
    assertBetween(obj[x.key], 0, 4);
  }
  assertEquals('c', x.key);
});

test('for-in with crazy lhs member expression on object', function() {
  var obj = { a: 1, b: 2, c: 3 };
  var x = { y: { z: { key: null } } };

  for (x.y.z['k' + 'e' + 'y'] in obj) {
    assertBetween(obj[x.y.z.key], 0, 4);
  }
  assertEquals('c', x.y.z.key);
});
