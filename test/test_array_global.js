// test_array_global.js
// --------------------

(this.load || require)((this.load ? 'test' : '.') + '/tools/assertions.js');


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
  assertEquals(4, a.pop());
  assertEquals(3, a.length);
  assertEquals(3, a.pop());
  assertEquals(2, a.length);
});

test('Array#push(element1, ..., elementN)', function() {
  var a = [1, 2];
  assertEquals(3, a.push('bears'));
  assertEquals(3, a.length);
  assertEquals(7, a.push('and', 'tigers', 'and', 'lions'));
  assertEquals(7, a.length);
});

test('Array#shift()', function() {
  var a = [1, 2, 'bears', 'and', 'tigers', 'and', 'lions'];
  assertEquals(1, a.shift());
  assertEquals(6, a.length);
  assertEquals(2, a.shift());
  assertEquals(5, a.length);
});

test('Array#unshift(element1, ..., elementN)', function() {
  var a = ['bears', 'and', 'tigers', 'and', 'lions'];
  assertEquals(6, a.unshift('oh'));
  assertEquals(6, a.length);
  assertEquals(8, a.unshift('!', 'my'));
  assertEquals(8, a.length);
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
  assertEquals('watwatwat',    Array(4).join('wat'));
  assertEquals('wat1wat1wat1', Array(4).join('wat' + 1));
  assertEquals('NaNNaNNaN',    Array(4).join('wat' - 1));
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

  assertEquals(0,  a.indexOf(4));
  assertEquals(0,  a.indexOf(4, -5));
  assertEquals(-1, a.indexOf(2, -3));
  assertEquals(1,  a.indexOf(2));
  assertEquals(2,  a.indexOf(1, -3));
  assertEquals(3,  a.indexOf(3));
  assertEquals(-1, a.indexOf(999));
});

test('Array#lastIndexOf()', function() {
  var a = [4, 2, 1, 3, 4];

  assertEquals(4,  a.lastIndexOf(4));
  assertEquals(0,  a.lastIndexOf(4, -2));
  assertEquals(0,  a.lastIndexOf(4, -4));
  assertEquals(-1, a.lastIndexOf(4, -6));
  assertEquals(1,  a.lastIndexOf(2));
  assertEquals(2,  a.lastIndexOf(1, -2));
  assertEquals(3,  a.lastIndexOf(3));
  assertEquals(-1, a.lastIndexOf(999));
});

test('Array#splice(index, howMany[, element1[, ...[, elementN]]])', function() {
  var a = ['angel', 'clown', 'mandarin', 'surgeon'], removed;

  removed = a.splice(2, 0, 'drum');
  assertEquals(5, a.length);
  assertArrayEquals([], removed);

  removed = a.splice(3, 1);
  assertEquals(4, a.length);
  assertEquals(1, removed.length);
  assertArrayEquals(['mandarin'], removed);

  removed = a.splice(2, 1, 'trumpet');
  assertEquals(4, a.length);
  assertArrayEquals(['drum'], removed);

  removed = a.splice(0, 2, 'parrot', 'anemone', 'blue');
  assertEquals(5, a.length);
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
  assertEquals(10, a.reduce(f5));
  assertEquals(20, a.reduce(f5, 10));

  assertEquals(5, [].reduce(f5, 5));
  assertEquals(5, [].reduce(f4, 5));

  assertEquals(5, [5].reduce(f5));
  assertEquals(5, [5].reduce(f4));
});

test('Array#reduceRight(callback[, seed])', function() {
  var a = [0, 1, 2, 3, 4];
  assertEquals(10, a.reduceRight(f5));
  assertEquals(20, a.reduceRight(f5, 10));

  assertEquals(5, [].reduceRight(f5, 5));
  assertEquals(5, [].reduceRight(f4, 5));

  assertEquals(5, [5].reduceRight(f5));
  assertEquals(5, [5].reduceRight(f4));
});
