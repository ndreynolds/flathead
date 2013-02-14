// test_array_global.js
// --------------------

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};

var assertArrayEquals = function(a, b) {
  assert(a.length === b.length);
  for (var i = 0; i < a.length; i++) {
    assert(a[i] === b[i]);
  }
};

var test = function(name, f) {
  f();
};


// ----------------------------------------------------------------------------
// Array Global
// ----------------------------------------------------------------------------

assert(Array);
assert(typeof Array === 'function');


test('Constructor', function() {

  test('new Array(arrayLength)', function() {
    assertArrayEquals([], new Array);
    assertArrayEquals([], new Array());
    assertArrayEquals([], new Array(0));
    assertArrayEquals([undefined, undefined], new Array(2));
    assertEquals(100, (new Array(100)).length);
    assertEquals(20000, (new Array(20000)).length);
    // assertEquals(4294967295, (new Array(4294967295)).length);
    // TODO: assert throws range error
  });

  test('new Array(element0, element1, ..., elementN)', function() {
    assertArrayEquals(['abc'], new Array('abc'));
    assertArrayEquals([1, 2, 3, 4], new Array(1, 2, 3, 4));
    assertArrayEquals([1, 'b', true], new Array(1, 'b', true));
    var a = new Array(0, 1, 2);
    assertArrayEquals([0, 1, 2], a);
  });

});

test('Array.isArray(value)', function() {
  assert(Array.isArray([]));
  assert(Array.isArray([1]));
  assert(Array.isArray(new Array()));
  assert(Array.isArray(Array.prototype));

  assert(!Array.isArray());
  assert(!Array.isArray({}));
  assert(!Array.isArray(null));
  assert(!Array.isArray(undefined));
  assert(!Array.isArray(17));
  assert(!Array.isArray('Array'));
  assert(!Array.isArray(true));
  assert(!Array.isArray(false));
});


// ----------------------------------------------------------------------------
// Array Prototype
// ----------------------------------------------------------------------------

assert(Array.prototype);
assertEquals('object', typeof Array.prototype);
assertEquals(0, Array.prototype.length);

test('Array#pop()', function() {
  var a = [1, 2, 3, 4];
  assert(a.pop() === 4);
  assert(a.length === 3);
  assert(a.pop() === 3);
  assert(a.length === 2);
});

test('Array#push(element1, ..., elementN)', function() {
  var a = [1, 2];
  assert(a.push('bears') === 3);
  assert(a.length === 3);
  assert(a.push('and', 'tigers', 'and', 'lions') === 7);
  assert(a.length === 7);
});

test('Array#shift()', function() {
  var a = [1, 2, 'bears', 'and', 'tigers', 'and', 'lions'];
  assert(a.shift() === 1);
  assert(a.length === 6);
  assert(a.shift() === 2);
  assert(a.length === 5);
});

test('Array#unshift(element1, ..., elementN)', function() {
  var a = ['bears', 'and', 'tigers', 'and', 'lions'];
  assert(a.unshift('oh') === 6);
  assert(a.length === 6);
  assert(a.unshift('!', 'my') === 8);
  assert(a.length === 8);
  // Just for fun
  assertEquals('lions and tigers and bears oh my !', a.reverse().join(' '));
});

test('Array#reverse()', function() {
  // Even and odd length arrays.
  var a1 = ['a', 'b', 'c', 'd'];
  var a2 = ['c', 'a', 't'];

  var result = a1.reverse();
  assertArrayEquals(['d', 'c', 'b', 'a'], result);
  assertArrayEquals(['d', 'c', 'b', 'a'], a1);

  a2.reverse();
  assertArrayEquals(['t', 'a', 'c'], a2);
});

var concat;

test('Array#concat(value1, value2, ..., valueN)', function() {
  var a = ['t', 'a', 'c'];
  concat = a.concat('t', ['f', 'u', 'l'], 'n', 'e', ['s', 's']);
  assertArrayEquals(['t', 'a', 'c', 't', 'f', 'u', 'l', 'n', 'e', 's', 's'], concat);
});

test('Array#join(sep)', function() {
  assert(concat.join('') === 'tactfulness');

  var a1 = ['foo', 'bar'];
  assertEquals('foobar', a1.join(''));
  assertEquals('foo,bar', a1.join(','));
  assertEquals('foobazbar', a1.join('baz'));

  var a2 = [1, 2, 3, 4, 5, 6, 7, 8, 9];
  assertEquals('1,2,3,4,5,6,7,8,9', a2.join(','));

  var a3 = ['a', 'b', 'c'];
  assertEquals('a,b,c', a3.join());

  // Examples from WAT talk
  assertEquals('watwatwat', Array(4).join('wat'));
  assertEquals('wat1wat1wat1', Array(4).join('wat' + 1));
  assertEquals('NaNNaNNaN', Array(4).join('wat' - 1));
});

test('Array#slice(begin[, end])', function() {
  var a = [1, 2, 3, 4, 5, 6, 7, 8, 9];

  var slice1 = a.slice(6);
  assertArrayEquals([7, 8, 9], slice1);

  var slice2 = a.slice(0, 2);
  assertArrayEquals([1, 2], slice2);

  var slice3 = a.slice(1, -5);
  assertArrayEquals([2, 3, 4], slice3);

  var slice4 = a.slice(99);
  assertArrayEquals([], slice4);

  var slice5 = a.slice(-50, 100);
  assertArrayEquals([1, 2, 3, 4, 5, 6, 7, 8, 9], slice5);

  var slice6 = a.slice(4, 2);
  assertArrayEquals([], slice6);

  var slice7 = a.slice(1, -20);
  assertArrayEquals([], slice7);

  var slice8 = a.slice(2, 2);
  assertArrayEquals([], slice8);
});

test('Array#indexOf()', function() {
  var a = [4, 2, 1, 3, 4];

  assert(a.indexOf(4) === 0);
  assert(a.indexOf(4, -5) === 0);
  assert(a.indexOf(2, -3) === -1);
  assert(a.indexOf(2) === 1);
  assert(a.indexOf(1, -3) === 2);
  assert(a.indexOf(3) === 3);
  assert(a.indexOf(999) === -1);
});

test('Array#lastIndexOf()', function() {
  var a = [4, 2, 1, 3, 4];

  assert(a.lastIndexOf(4) === 4);
  assert(a.lastIndexOf(4, -2) === 0);
  assert(a.lastIndexOf(4, -4) === 0);
  assert(a.lastIndexOf(4, -6) === -1);
  assert(a.lastIndexOf(2) === 1);
  assert(a.lastIndexOf(1, -2) === 2);
  assert(a.lastIndexOf(3) === 3);
  assert(a.lastIndexOf(999) === -1);
});

test('Array#splice(index, howMany[, element1[, ...[, elementN]]])', function() {
  var a = ['angel', 'clown', 'mandarin', 'surgeon'], removed;

  removed = a.splice(2, 0, 'drum');
  assert(a.length === 5);
  assertArrayEquals([], removed);

  removed = a.splice(3, 1);
  assert(a.length === 4);
  assert(removed.length === 1);
  assertArrayEquals(['mandarin'], removed);

  removed = a.splice(2, 1, 'trumpet');
  assert(a.length === 4);
  assertArrayEquals(['drum'], removed);

  removed = a.splice(0, 2, 'parrot', 'anemone', 'blue');
  assert(a.length === 5);
  assertArrayEquals(['angel', 'clown'], removed);
});

test('Array#sort()', function() {
  [].sort();

  // TODO: expand these tests
  var a = [4, 2, 1, 3];
  a.sort();
  assertArrayEquals([1, 2, 3, 4], a);
  assertArrayEquals([200, 45, 7], [7, 45, 200].sort());
});

test('Array#forEach(callback[, ctx])', function() {
  var a = [1, 2, 3, 4];
  var sum = 0;
  var f = function(x) { return sum += x; };

  a.forEach(f, this);
  assertEquals(10, sum);
});

var f1 = function(x) { return x < 5; };
var f2 = function(x) { return x % 2 === 0; };
var f3 = function(x) { return x < 0; };
var f4 = function(x) { return x * x; };
var f5 = function(prev, cur, index, arr) {
  return prev + cur;
};

test('Array#every(callback[, ctx])', function() {
  var a = [1, 2, 3, 4];

  assert(a.every(f1, this));
  assert(!a.every(f2, this));
});

test('Array#some(callback[, ctx])', function() {
  var a = [1, 2, 3, 4];

  assert(a.some(f1, this));
  assert(a.some(f2, this));
  assert(!a.some(f3, this));
});

test('Array#filter(callback[, ctx])', function() {
  var a = [1, 2, 3, 4];

  var result = a.filter(f2, this);
  assertArrayEquals([2, 4], result);
});

test('Array#map(callback[, ctx])', function() {
  var a = [1, 2, 3, 4];

  var result = a.map(f4, this);
  assertArrayEquals([1, 4, 9, 16], result);
});

test('Array#reduce(callback[, seed])', function() {
  var a = [0, 1, 2, 3, 4];
  assert(a.reduce(f5) === 10);
  assert(a.reduce(f5, 10) === 20);

  assert([].reduce(f5, 5) === 5);
  assert([].reduce(f4, 5) === 5);

  assert([5].reduce(f5) === 5);
  assert([5].reduce(f4) === 5);
});

test('Array#reduceRight(callback[, seed])', function() {
  var a = [0, 1, 2, 3, 4];
  assert(a.reduceRight(f5) === 10);
  assert(a.reduceRight(f5, 10) === 20);

  assert([].reduceRight(f5, 5) === 5);
  assert([].reduceRight(f4, 5) === 5);

  assert([5].reduceRight(f5) === 5);
  assert([5].reduceRight(f4) === 5);
});
