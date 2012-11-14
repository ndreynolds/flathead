// test_arrays.js
// --------------

var assert = console.assert;
var assertEquals = function(a, b) {
  assert(a === b);
};

var test = function(name, f) {
  f();
};


test('simple homogenous arrays', function() {
  var arr = [1, 2, 3];
  assertEquals(1, arr[0]);
  assertEquals(2, arr[1]);
  assertEquals(3, arr[2]);
});

test('heterogeneous arrays', function() {
  var arr = [1, 'Dog', false, null, undefined, [1, 2, 3], {a: 12}];
  assertEquals(1, arr[0]);
  assertEquals('Dog', arr[1]);
  assertEquals(false, arr[2]);
  assertEquals(null, arr[3]);
  assertEquals(undefined, arr[4]);
  assertEquals(1, arr[5][0]);
  assertEquals(2, arr[5][1]);
  assertEquals(3, arr[5][2]);
  assertEquals(12, arr[6].a);
});

test('nested arrays', function() {
  var arr = [[[[[[1, 2]]]]]];
  assertEquals(1, arr[0][0][0][0][0][0]);
  assertEquals(2, arr[0][0][0][0][0][1]);
});

test('expression index', function() {
  var getIndex = function() { return 1; };
  var a4 = ['a', 'b', 'c', 'd'];
  assertEquals('c', a4[4 - 2]);
  assertEquals('b', a4[getIndex()]);
});

test('bracket assignment', function() {
  var arr = [];

  arr[0] = 10;
  arr[1] = 20;
  arr[2] = 30;
  arr[3] = 40;

  assertEquals(4, arr.length);
  assertEquals(10, arr[0]);
  assertEquals(20, arr[1]);
  assertEquals(30, arr[2]);
  assertEquals(40, arr[3]);
});

test('indices do not need to be assigned in order (or at all)', function() {
  var arr = [];

  arr[12] = 42;
  arr[10002] = 'hello';

  assertEquals(10003, arr.length);
  assertEquals(42, arr[12]);
  assertEquals('hello', arr[10002]);
});

test('big arrays', function() {
  var arr = [];
  arr[Math.pow(2, 31) - 1] = 42;
  assertEquals(Math.pow(2, 31), arr.length);
});
