// test_date_global.js
// -------------------
// Tests for the Date global object.

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};

var assertEqualsWhen = function(a, b, tzOffset) {
  if ((new Date()).getTimezoneOffset() === tzOffset)
    assertEquals(a, b);
  else
    console.log('warning: tz-specific assertion skipped');
};

// This assertion will only be made when the timezone is EDT (offset +240 min)
var assertEqualsWhenEDT = function(a, b) {
  assertEqualsWhen(a, b, 240);
};


// ----------------------------------------------------------------------------
// Date Global
// ----------------------------------------------------------------------------

assert(Date);
assertEquals('function', typeof Date);

var now = Date.now();

// Constructor

assert((new Date).valueOf() >= now);
assert((new Date()).valueOf() >= now);
assert((new Date(0)).valueOf() < now);
assert((new Date(100)).valueOf() < now);


// Date.now()

assert(Date.now() > 1351474364385);
assert(Date.now() >= now);


// Date.parse()

// TODO


// Date.UTC()

// TODO


// ----------------------------------------------------------------------------
// Date Prototype
// ----------------------------------------------------------------------------

assert(Date.prototype);

// Thu, 01 Jan 1970 05:00:00 GMT
var d1 = new Date(1970, 0, 1, 0, 0, 0, 0);
// Mon, 07 May 1990 04:05:00 GMT
var d2 = new Date(1990, 4, 7, 0, 5, 0, 0);
// Wed, 28 Jun 2000 19:30:00 GMT
var d3 = new Date(2000, 5, 28, 15, 30, 0, 0);
// Thu, 25 Dec 2149 08:20:46 GMT
var d4 = new Date(2149, 11, 25, 3, 20, 46, 670);

console.log(d1.valueOf());
console.log(d2.valueOf());
console.log(d3.valueOf());
console.log(d4.valueOf());


// Date.prototype.getDate()
// Date.prototype.getUTCDate()

assertEqualsWhenEDT(1, d1.getDate());
assertEqualsWhenEDT(7, d2.getDate());
assertEqualsWhenEDT(28, d3.getDate());
assertEqualsWhenEDT(25, d4.getDate());

assertEquals(1, d1.getUTCDate());
assertEquals(7, d2.getUTCDate());
assertEquals(28, d3.getUTCDate());
assertEquals(25, d4.getUTCDate());


// Date.prototype.getDay()
// Date.prototype.getUTCDay()

assertEqualsWhenEDT(4, d1.getDay());
assertEqualsWhenEDT(1, d2.getDay());
assertEqualsWhenEDT(3, d3.getDay());
assertEqualsWhenEDT(4, d4.getDay());

assertEquals(4, d1.getUTCDay());
assertEquals(1, d2.getUTCDay());
assertEquals(3, d3.getUTCDay());
assertEquals(4, d4.getUTCDay());


// Date.prototype.getFullYear()
// Date.prototype.getUTCFullYear()

assertEqualsWhenEDT(1970, d1.getFullYear());
assertEqualsWhenEDT(1990, d2.getFullYear());
assertEqualsWhenEDT(2000, d3.getFullYear());
assertEqualsWhenEDT(2149, d4.getFullYear());

assertEquals(1970, d1.getUTCFullYear());
assertEquals(1990, d2.getUTCFullYear());
assertEquals(2000, d3.getUTCFullYear());
assertEquals(2149, d4.getUTCFullYear());


// Date.prototype.getHours()
// Date.prototype.getUTCHours()

assertEqualsWhenEDT(0, d1.getHours());
assertEqualsWhenEDT(0, d2.getHours());
assertEqualsWhenEDT(15, d3.getHours());
assertEqualsWhenEDT(3, d4.getHours());

assertEquals(5, d1.getUTCHours());
assertEquals(4, d2.getUTCHours());
assertEquals(19, d3.getUTCHours());
assertEquals(8, d4.getUTCHours());


// Date.prototype.getMilliseconds()
// Date.prototype.getUTCMilliseconds()

assertEqualsWhenEDT(0, d1.getMilliseconds());
assertEqualsWhenEDT(0, d2.getMilliseconds());
assertEqualsWhenEDT(0, d3.getMilliseconds());
assertEqualsWhenEDT(670, d4.getMilliseconds());

assertEquals(0, d1.getUTCMilliseconds());
assertEquals(0, d2.getUTCMilliseconds());
assertEquals(0, d3.getUTCMilliseconds());
assertEquals(670, d4.getUTCMilliseconds());


// Date.prototype.getMinutes()
// Date.prototype.getUTCMinutes()

assertEqualsWhenEDT(0, d1.getMinutes());
assertEqualsWhenEDT(5, d2.getMinutes());
assertEqualsWhenEDT(30, d3.getMinutes());
assertEqualsWhenEDT(20, d4.getMinutes());

assertEquals(0, d1.getUTCMinutes());
assertEquals(5, d2.getUTCMinutes());
assertEquals(30, d3.getUTCMinutes());
assertEquals(20, d4.getUTCMinutes());


// Date.prototype.getMonth()
// Date.prototype.getUTCMonth()

assertEqualsWhenEDT(0, d1.getMonth());
assertEqualsWhenEDT(4, d2.getMonth());
assertEqualsWhenEDT(5, d3.getMonth());
assertEqualsWhenEDT(11, d4.getMonth());

assertEquals(0, d1.getUTCMonth());
assertEquals(4, d2.getUTCMonth());
assertEquals(5, d3.getUTCMonth());
assertEquals(11, d4.getUTCMonth());


// Date.prototype.getSeconds()
// Date.prototype.getUTCSeconds()

assertEqualsWhenEDT(0, d1.getSeconds());
assertEqualsWhenEDT(0, d2.getSeconds());
assertEqualsWhenEDT(0, d3.getSeconds());
assertEqualsWhenEDT(46, d4.getSeconds());

assertEquals(0, d1.getUTCSeconds());
assertEquals(0, d2.getUTCSeconds());
assertEquals(0, d3.getUTCSeconds());
assertEquals(46, d4.getUTCSeconds());


// Date.prototype.getYear()

assertEqualsWhenEDT(70, d1.getYear());
assertEqualsWhenEDT(90, d2.getYear());
assertEqualsWhenEDT(100, d3.getYear());
assertEqualsWhenEDT(249, d4.getYear());
