// test_regexp_global.js
// ---------------------
// Tests for the RegExp global object.

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};

var assertRegExpEquals = function(a, b) {
  assertEquals(a.source, b.source);
  assertEquals(a.global, b.global);
  assertEquals(a.multiline, b.multiline);
  assertEquals(a.ignoreCase, b.ignoreCase);
};


// ----------------------------------------------------------------------------
// RegExp Global
// ----------------------------------------------------------------------------

assert(RegExp);
assertEquals('function', typeof RegExp);

// Constructor

assertEquals('object', typeof new RegExp);
assertEquals('object', typeof new RegExp());
assertEquals('object', typeof RegExp());
assertRegExpEquals(/(?:)/, new RegExp());
assertRegExpEquals(/(?:)/gim, new RegExp(undefined, 'igm'));
assertRegExpEquals(/null/gim, new RegExp(null, 'igm'));
assertRegExpEquals(/abc/i, new RegExp('abc', 'i'));
assertRegExpEquals(/abc/i, RegExp('abc', 'i'));
assertRegExpEquals(/abc/gim, new RegExp('abc', 'igm'));
assertRegExpEquals(/abc/ig, new RegExp('abc', 'gi'));
assertRegExpEquals(/abc/g, new RegExp('abc', 'g'));


// ----------------------------------------------------------------------------
// RegExp Prototype
// ----------------------------------------------------------------------------

assert(RegExp.prototype);
assertEquals('object', typeof RegExp.prototype);


// RegExp.prototype.exec(str)

// TODO


// RegExp.prototype.test(str)

// TODO


// RegExp.prototype.toString()

assertEquals('/abc/gim', (new RegExp('abc', 'img')).toString());
assertEquals('/abc/gi', (new RegExp('abc', 'ig')).toString());
assertEquals('/abc/m', (new RegExp('abc', 'm')).toString());
assertEquals('/abc/gim', /abc/img.toString());
