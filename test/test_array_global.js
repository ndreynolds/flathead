// test_array_global.js
// --------------------

var assert = console.assert;
var assertEquals = function(a, b) {
  assert(a === b);
};


// ----------------------------------------------------------------------------
// Array Global
// ----------------------------------------------------------------------------

assert(Array);

assert(Array.isArray([]));
assert(Array.isArray([1]));
// TODO: (waiting on new operator) assert(Array.isArray( new Array() );
assert(Array.isArray(Array.prototype));

assert(!Array.isArray());
assert(!Array.isArray({}));
assert(!Array.isArray(null));
assert(!Array.isArray(undefined));
assert(!Array.isArray(17));
assert(!Array.isArray("Array"));
assert(!Array.isArray(true));
assert(!Array.isArray(false));


// ----------------------------------------------------------------------------
// Array Prototype
// ----------------------------------------------------------------------------

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
// TODO: assert(a5.reverse().join(' ') === 'lions and tigers and bears oh my !');

// Even and odd length arrays.
var a6 = ['a', 'b', 'c', 'd'];
var a7 = ['c', 'a', 't'];

var result = a6.reverse();
assert(result[0] === a6[0]);

assert(a6[0] === 'd');
assert(a6[1] === 'c');
assert(a6[2] === 'b');
assert(a6[3] === 'a');

a7.reverse();
assert(a7[0] === 't');
assert(a7[1] === 'a');
assert(a7[2] === 'c');


// Array.prototype.concat(value1, value2, ..., valueN)

var concat = a7.concat('t', ['f', 'u', 'l'], 'n', 'e', ['s', 's']);
assert(concat[0] === 't');
assert(concat[3] === 't');
assert(concat[6] === 'l');
assert(concat[9] === 's');


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
assert(slice1.length === 3);
assert(slice1[0] === 7);
assert(slice1[1] === 8);
assert(slice1[2] === 9);

var slice2 = a9.slice(0, 2);
assert(slice2.length === 2);
assert(slice2[0] === 1);
assert(slice2[1] === 2);

var slice3 = a9.slice(1, -5);
assert(slice3.length === 3);
assert(slice3[0] === 2);
assert(slice3[1] === 3);
assert(slice3[2] === 4);

var slice4 = a9.slice(99);
assert(slice4.length === 0);

var slice5 = a9.slice(-50, 100);
assert(slice5.length === 9);
assert(slice5[0] === 1);
assert(slice5[8] === 9);


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

var a11 = ["angel", "clown", "mandarin", "surgeon"], removed;

removed = a11.splice(2, 0, "drum");
assert(a11.length === 5);
assert(removed.length === 0);

removed = a11.splice(3, 1);
assert(a11.length === 4);
assert(removed.length === 1);
assert(removed[0] === "mandarin");

removed = a11.splice(2, 1, "trumpet");
assert(a11.length === 4);
assert(removed.length === 1);
assert(removed[0] === "drum");

removed = a11.splice(0, 2, "parrot", "anemone", "blue");
assert(a11.length === 5);
assert(removed.length === 2);
assert(removed[0] === "angel");
assert(removed[1] === "clown");


// Array.prototype.sort()
// TODO
// a11.sort();
// assert(a11[0] === 1);
// assert(a11[1] === 2);
// assert(a11[2] === 3);
// assert(a11[3] === 4);


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
assert(result[0] === 2);
assert(result[1] === 4);
assert(result.length === 2);


// Array.prototype.map(callback[, ctx])

var f4 = function(x) { return x * x; };

var result = a13.map(f4, this);
assert(result[0] === 1);
assert(result[1] === 4);
assert(result[2] === 9);
assert(result[3] === 16);


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
