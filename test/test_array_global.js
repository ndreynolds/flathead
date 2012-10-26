// test_array_global.js
// --------------------

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};

var assertArrayEquals = function(a, b) {
  if (a.length !== b.length)
    return false;
  for (var i = 0; i < a.length; i++) {
    assert(a[i] === b[i]);
  }
};


// ----------------------------------------------------------------------------
// Array Global
// ----------------------------------------------------------------------------

assert(Array);
assert(typeof Array === 'function');


// Constructor

// new Array(arrayLength)
assertArrayEquals([], new Array);
assertArrayEquals([], new Array());
assertArrayEquals([], new Array(0));
assertArrayEquals([undefined, undefined], new Array(2));
assertEquals(100, (new Array(100)).length);
assertEquals(20000, (new Array(20000)).length);
// assertEquals(4294967295, (new Array(4294967295)).length);
// TODO: assert throws range error

// new Array(element0, element1, ..., elementN)
assertArrayEquals(['abc'], new Array('abc'));
assertArrayEquals([1, 2, 3, 4], new Array(1, 2, 3, 4));
assertArrayEquals([1, 'b', true], new Array(1, 'b', true));
var a = new Array(0, 1, 2);
assertArrayEquals([0, 1, 2], a);


// Array.isArray(value)

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


// ----------------------------------------------------------------------------
// Array Prototype
// ----------------------------------------------------------------------------

assert(Array.prototype);
assertEquals('object', typeof Array.prototype);
assertEquals(0, Array.prototype.length);


var a5 = [1, 2, 3, 4];

// Array.prototype.pop()

assert(a5.pop() === 4);
assert(a5.length === 3);
assert(a5.pop() === 3);
assert(a5.length === 2);


// Array.prototype.push(element1, ..., elementN)

assert(a5.push('bears') === 3);
assert(a5.length === 3);
assert(a5.push('and', 'tigers', 'and', 'lions') === 7);
assert(a5.length === 7);


// Array.prototype.shift()

assert(a5.shift() === 1);
assert(a5.length === 6);
assert(a5.shift() === 2);
assert(a5.length === 5);


// Array.prototype.unshift(element1, ..., elementN)

assert(a5.unshift('oh') === 6);
assert(a5.length === 6);
assert(a5.unshift('!', 'my') === 8);
assert(a5.length === 8);


// Array.prototype.reverse()

// Just for fun.
// FIXME: assert(a5.reverse().join(' ') === 'lions and tigers and bears oh my !');

// Even and odd length arrays.
var a6 = ['a', 'b', 'c', 'd'];
var a7 = ['c', 'a', 't'];

var result = a6.reverse();
assertArrayEquals(['d', 'c', 'b', 'a'], result);
assertArrayEquals(['d', 'c', 'b', 'a'], a6);

a7.reverse();
assertArrayEquals(['t', 'a', 'c'], a7);


// Array.prototype.concat(value1, value2, ..., valueN)

var concat = a7.concat('t', ['f', 'u', 'l'], 'n', 'e', ['s', 's']);
assertArrayEquals(['t', 'a', 'c', 't', 'f', 'u', 'l', 'n', 'e', 's', 's'], concat);


// Array.prototype.join(sep)

assert(concat.join('') === 'tactfulness');

var a8 = ['foo', 'bar'];
assert(a8.join('') === 'foobar');
assert(a8.join(',') === 'foo,bar');
assert(a8.join('baz') === 'foobazbar');

var a9 = [1,2,3,4,5,6,7,8,9];
assert(a9.join(',') === '1,2,3,4,5,6,7,8,9');


// Array.prototype.slice(begin[, end])

var slice1 = a9.slice(6);
assertArrayEquals([7, 8, 9], slice1);

var slice2 = a9.slice(0, 2);
assertArrayEquals([1, 2], slice2);

var slice3 = a9.slice(1, -5);
assertArrayEquals([2, 3, 4], slice3);

var slice4 = a9.slice(99);
assertArrayEquals([], slice4);

var slice5 = a9.slice(-50, 100);
assertArrayEquals([1, 2, 3, 4, 5, 6, 7, 8, 9], slice5);


// Array.prototype.indexOf()

var a10 = [4, 2, 1, 3, 4];

assert(a10.indexOf(4) === 0);
assert(a10.indexOf(4, -5) === 0);
assert(a10.indexOf(2, -3) === -1);
assert(a10.indexOf(2) === 1);
assert(a10.indexOf(1, -3) === 2);
assert(a10.indexOf(3) === 3);
assert(a10.indexOf(999) === -1);


// Array.prototype.lastIndexOf()

assert(a10.lastIndexOf(4) === 4);
assert(a10.lastIndexOf(4, -2) === 0);
assert(a10.lastIndexOf(4, -4) === 0);
assert(a10.lastIndexOf(4, -6) === -1);
assert(a10.lastIndexOf(2) === 1);
assert(a10.lastIndexOf(1, -2) === 2);
assert(a10.lastIndexOf(3) === 3);
assert(a10.lastIndexOf(999) === -1);


// Array.prototype.splice(index, howMany[, element1[, ...[, elementN]]])

var a11 = ['angel', 'clown', 'mandarin', 'surgeon'], removed;

removed = a11.splice(2, 0, 'drum');
assert(a11.length === 5);
assertArrayEquals([], removed);

removed = a11.splice(3, 1);
assert(a11.length === 4);
assert(removed.length === 1);
assertArrayEquals(['mandarin'], removed);

removed = a11.splice(2, 1, 'trumpet');
assert(a11.length === 4);
assertArrayEquals(['drum'], removed);

removed = a11.splice(0, 2, 'parrot', 'anemone', 'blue');
assert(a11.length === 5);
assertArrayEquals(['angel', 'clown'], removed);


// Array.prototype.sort()

// TODO: expand these tests
a11.sort();
assertArrayEquals([1, 2, 3, 4], a11);
assertArrayEquals([200, 45, 7], [7, 45, 200].sort());


// Array.prototype.forEach(callback[, ctx])

var a12 = [1, 2, 3, 4];
var sum = 0;
var f = function(x) { return sum += x; };

a12.forEach(f, this);
assertEquals(10, sum);


// Array.prototype.every(callback[, ctx])

var a13 = [1, 2, 3, 4];
var f1 = function(x) { return x < 5; };
var f2 = function(x) { return x % 2 === 0; };

assert(a13.every(f1, this));
assert(!a13.every(f2, this));


// Array.prototype.some(callback[, ctx])

var f3 = function(x) { return x < 0; };

assert(a13.some(f1, this));
assert(a13.some(f2, this));
assert(!a13.some(f3, this));


// Array.prototype.filter(callback[, ctx])

var result = a13.filter(f2, this);
assertArrayEquals([2, 4], result);


// Array.prototype.map(callback[, ctx])

var f4 = function(x) { return x * x; };

var result = a13.map(f4, this);
assertArrayEquals([1, 4, 9, 16], result);


// Array.prototype.reduce(callback[, seed])

var a14 = [0, 1, 2, 3, 4];
var f5 = function(prev, cur, index, arr) {
  return prev + cur;
};
assert(a14.reduce(f5) === 10);
assert(a14.reduce(f5, 10) === 20);


// Array.prototype.reduceRight(callback[, seed])

assert(a14.reduceRight(f5) === 10);
assert(a14.reduceRight(f5, 10) === 20);
