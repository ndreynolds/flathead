// test_string_global.js
// ---------------------
// Tests for the String global object.

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
// String Global
// ----------------------------------------------------------------------------

assert(String);
assertEquals('function', typeof String);

test('Constructor', function() {
  assertEquals('object', typeof new String);
  assertEquals('object', typeof new String());
  assertEquals('object', typeof (new String()));
  assertEquals('object', typeof (new String('abc')));
  assertEquals('', (new String('')).valueOf());
  assertEquals('abc', (new String('abc')).valueOf());
  assertEquals('hello', (new String('hello    ')).trim());
});

test('String.fromCharCode(charCode)', function() {
  assert(String.fromCharCode);
});


// ----------------------------------------------------------------------------
// String Prototype
// ----------------------------------------------------------------------------

assert(String.prototype);
assertEquals('object', typeof String.prototype);

test('String#charAt(index)', function() {
  var s = 'abc';
  assertEquals('a', s.charAt());
  assertEquals('a', s.charAt(0));
  assertEquals('b', s.charAt(1));
  assertEquals('c', s.charAt(2));
  assertEquals('',  s.charAt(999));
  assertEquals('',  s.charAt(-1));
});

test('String#charCodeAt(index)', function() {
  var s = 'abc';
  assert(s.charCodeAt);
});

test('String#concat(str2, str3[, ..., stringN])', function() {
  var s = 'Hello, ';
  assertEquals('Hello, Kevin have a nice day.', s.concat('Kevin', ' have a nice day.'));
  assertEquals('Hello, ', s);
});

test('String#indexOf(searchValue[, fromIndex])', function() {
  var s = 'Blue Whale';
  assertEquals(0,  s.indexOf('Blue'));
  assertEquals(0,  s.indexOf('Blue', -1000));
  assertEquals(-1, s.indexOf('Blute'));
  assertEquals(5,  s.indexOf('Whale', 0));
  assertEquals(5,  s.indexOf('Whale', 5));
  assertEquals(9,  s.indexOf('', 9));
  assertEquals(10, s.indexOf('', 10));
  assertEquals(10, s.indexOf('', 11));
  assertEquals(-1, s.indexOf('blue', 11));
});

test('String#lastIndexOf(searchValue[, fromIndex])', function() {
  var s = 'canal';
  assertEquals(3,  s.lastIndexOf('a'));
  assertEquals(1,  s.lastIndexOf('a', 2));
  assertEquals(-1, s.lastIndexOf('a', 0));
  assertEquals(-1, s.lastIndexOf('x'));
  assertEquals(-1, s.lastIndexOf('A'));

  s = 'test test test';
  assertEquals(10, s.lastIndexOf('test', Infinity));
  assertEquals(10, s.lastIndexOf('test', s.length * 2));
  assertEquals(10, s.lastIndexOf('test', 15));
  assertEquals(10, s.lastIndexOf('test', 14));
  assertEquals(10, s.lastIndexOf('test', 10));
  assertEquals(5,  s.lastIndexOf('test', 9));
  assertEquals(5,  s.lastIndexOf('test', 6));
  assertEquals(5,  s.lastIndexOf('test', 5));
  assertEquals(0,  s.lastIndexOf('test', 4));
  assertEquals(0,  s.lastIndexOf('test', 0));
  assertEquals(0,  s.lastIndexOf('test', -1));
  assertEquals(0,  s.lastIndexOf('test', -s.length));
  assertEquals(0,  s.lastIndexOf('test', -Infinity));
  assertEquals(10, s.lastIndexOf('test'));
  assertEquals(-1, s.lastIndexOf('notpresent'));
  assertEquals(-1, s.lastIndexOf());
  assertEquals(10, s.lastIndexOf('test', 'not a number'));
});

test('String#localeCompare(compareString)', function() {
  var s = 'abcd';
  assert(s.localeCompare('abcd') === 0);
  assert(s.localeCompare('abc')   > 0);
  assert(s.localeCompare('abcde') < 0);
  assert(s.localeCompare('abcx')  < 0);
  assert(s.localeCompare('abtd')  < 0);
  assert(s.localeCompare('aacd')  > 0);
});

test('String#match(regexp)', function() {
  var s, re, matches;
  s = "For more information, see Chapter 3.4.5.1";
  re = /(chapter \d+(\.\d)*)/i;
  matches = s.match(re);
  assertArrayEquals(['Chapter 3.4.5.1', 'Chapter 3.4.5.1', '.1'], matches);

  s = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  re = /[A-E]/gi;
  matches = s.match(re);
  assertArrayEquals(['A', 'B', 'C', 'D', 'E', 'a', 'b', 'c', 'd', 'e'], matches);
});

test('String#replace(regexp|substr, newSubStr|function[, flags])', function() {

  // No replace possible
  assertEquals('apple', 'apple'.replace('strawberry', 'orange'));

  // Simple strings
  assertEquals('orange',     'apple'.replace('apple', 'orange'));
  assertEquals('pineapple',  'apple'.replace('a', 'pinea'));
  assertEquals('grapefruit', 'grapes'.replace('s', 'fruit'));
  assertEquals('arple',      'apple'.replace('p', 'r'));

  // RegExp search, string replacement
  assertEquals('orange', 'APPLE'.replace(/apple/i, 'orange'));
  assertEquals('arple',  'apple'.replace(/p/, 'r'));
  assertEquals('arrle',  'apple'.replace(/p/g, 'r'));
  assertEquals('',       'ABCDEF'.replace(/[a-f]/gi, ''));
  assertEquals('',       'ABCDEF'.replace(/[a-f]+/i, ''));

  assertEquals('xb',     'xyxyxyxbxyxyxyxy'.replace(/xy/g, ''));
  assertEquals('axbxcx', 'xaxbxcx'.replace('x', ''));
  assertEquals('axbxcx', 'xaxbxcx'.replace(/x/, ''));
  assertEquals('abc',    'xaxbxcx'.replace(/x/g, ''));

  assertEquals('xaxxcx', 'xaxbxcx'.replace('b', ''));
  assertEquals('xaxxcx', 'xaxbxcx'.replace(/b/, ''));
  assertEquals('xaxxcx', 'xaxbxcx'.replace(/b/g, ''));
});

test('String#search(regexp)', function() {
  var s = 'abcFOOBARdef';
  assertEquals(3,  s.search(/FOO/));
  assertEquals(3,  s.search(/foo/i));
  assertEquals(6,  s.search(/BAR/));
  assertEquals(6,  s.search(/bar/i));
  assertEquals(-1, s.search(/baz/));

  assertEquals(1,  'baz'.search('a'));
  assertEquals(-1, 'baz'.search('k'));
});

test('String#slice(beginSlice[, endSlice])', function() {
  var s = 'slice me';
  assertEquals('slice me', s.slice(0));
  assertEquals('ice me',   s.slice(2));
  assertEquals('sl',       s.slice(0, 2));
  assertEquals('slic',     s.slice(0, 4));
  assertEquals('ic',       s.slice(2, 4));
  assertEquals('slice me', s.slice(0, 24));
  assertEquals('ice me',   s.slice(2, 100));
  assertEquals('slice',    s.slice(0, -3));
  assertEquals('',         s.slice(0, -500));
  assertEquals('',         s.slice(-500, -500));
  assertEquals('sl',       s.slice(-500, 2));
});

test('String#split([separator][, limit])', function() {

  var s = 'abc,def,ghi';
  assertArrayEquals(['abc,def,ghi'],        s.split());
  assertArrayEquals([],                     s.split(',', 0));
  assertArrayEquals(['abc', 'def', 'ghi'],  s.split(','));
  assertArrayEquals(['abc', 'def'],         s.split(',', 2));

  assertArrayEquals([],                     ''.split(''));
  assertArrayEquals([''],                   ''.split());
  assertArrayEquals([],                     'abc'.split('', 0));
  assertArrayEquals(['a', 'b', 'c', 'd'],   'abcd'.split(''));
  assertArrayEquals(['a', 'b', 'c'],        'aXbXc'.split('X'));
  assertArrayEquals(['a', 'b', 'c'],        'aXXbXXc'.split('XX'));
  assertArrayEquals(['a', 'b', 'c', ''],    'aXbXcX'.split('X'));
  assertArrayEquals(['a', 'b', 'c'],        'a1b1c'.split(1));

  assertArrayEquals(['as','fas','fas','f'], 'asdfasdfasdf'.split('d'));
  assertArrayEquals(['as','fas','fas','f'], 'asdfasdfasdf'.split('d', -1));
  assertArrayEquals(['as','fas','fas',''],  'asdfasdfasd'.split('d'));
  assertArrayEquals(['aXbXc'],              'aXbXc'.split(/Y/));
  assertArrayEquals(['a', 'b', 'c'],        'aXbXc'.split(/X/));
  assertArrayEquals(['a', 'b', 'c'],        'aXbXc'.split(/X/g));
  assertArrayEquals(['a', 'b', 'c'],        'aXbXc'.split(/x/i));
  assertArrayEquals(['a', 'b', 'c', ''],    'aXbXcX'.split(/X/));
  assertArrayEquals(['a', 'b'],             'aXXb'.split(/X+/));
  // FIXME assertArrayEquals(['a', 'b'],             'aXXb'.split(/X*/));
  assertArrayEquals(['a', '', 'b'],         'aXXb'.split(/X/));
  // FIXME assertArrayEquals(['a', 'b'],             'ab'.split(/a*?/));
  // FIXME assertArrayEquals(['', 'b'],              'ab'.split(/a*/));
});

test('String#substr(start[, length])', function() {
  var s = 'abcdefghij';
  assertEquals('bc',        s.substr(1, 2));
  assertEquals('hi',        s.substr(-3, 2));
  assertEquals('hij',       s.substr(-3));
  assertEquals('bcdefghij', s.substr(1));
  // FIXME assertEquals('bcdefghij', s.substr(1, Infinity));
  assertEquals('ab',        s.substr(-20, 2));
  assertEquals('',          s.substr(20, 2));
});

test('String#substring(start[, end])', function() {
  var s = 'JavaScript';
  assertEquals('Jav',        s.substring(0, 3));
  assertEquals('Jav',        s.substring(3, 0));
  assertEquals('Scr',        s.substring(4, 7));
  assertEquals('Scr',        s.substring(7, 4));
  assertEquals('Java',       s.substring(0, 4));
  assertEquals('JavaScript', s.substring(0, 10));
  assertEquals('JavaScript', s.substring(10, 0));
  assertEquals('',           s.substring(1000, 1000));
  assertEquals('JavaScript', s.substring(-999, 1000));
  assertEquals('',           s.substring(-999, -999));
});

test('String#toLocaleLowerCase()', function() {
  assertEquals('alphabet',  'ALPHABET'.toLocaleLowerCase());
  assertEquals('alphabet',  'alphabet'.toLocaleLowerCase());
  assertEquals('hello!!11', 'HEllo!!11'.toLocaleLowerCase());
});

test('String#toLowerCase()', function() {
  assertEquals('alphabet',  'ALPHABET'.toLowerCase());
  assertEquals('alphabet',  'alphabet'.toLowerCase());
  assertEquals('hello!!11', 'HEllo!!11'.toLowerCase());
});

test('String#toLocaleUpperCase()', function() {
  assertEquals('ALPHABET', 'alphabet'.toLocaleUpperCase());
  assertEquals('ALPHABET', 'ALPHABET'.toLocaleUpperCase());
  assertEquals('HELLO!!11', 'Hello!!11'.toLocaleUpperCase());
});

test('String#toUpperCase()', function() {
  assertEquals('ALPHABET', 'alphabet'.toUpperCase());
  assertEquals('ALPHABET', 'ALPHABET'.toUpperCase());
  assertEquals('HELLO!!11', 'Hello!!11'.toUpperCase());
});

test('String#toString()', function() {
  var s = "I'm a string";
  assertEquals("I'm a string", s.toString());
});

test('String#valueOf()', function() {
  var s = "I'm a string";
  assertEquals("I'm a string", s.valueOf());
});

test('String#trim(Right|Left)?()', function() {
  var s = '    lots of whitespace        ';
  assertEquals('lots of whitespace',         s.trim());
  assertEquals('lots of whitespace        ', s.trimLeft());
  assertEquals('    lots of whitespace',     s.trimRight());
});
