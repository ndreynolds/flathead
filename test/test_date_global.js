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

assertEqualsWhenEDT(18000000, (new Date(1970, 0, 1, 0, 0, 0, 0)).valueOf());

// These could fail on a slow system or implementation. Need a better test.
assertEquals((new Date(Date.now())).toString(), Date());
assertEquals((new Date(Date.now())).toString(), Date(10041));
assertEquals((new Date(Date.now())).toString(), Date(1970, 0, 1, 0, 0, 0, 0));


// Date.now()

assert(Date.now() > 1351474364385);
assert(Date.now() >= now);


// Date.parse()

// TODO


// Date.UTC()

assertEquals(0, Date.UTC(1970, 0, 1, 0, 0, 0, 0));
assertEquals(642038700000, Date.UTC(1990, 4, 7, 0, 5, 0, 0));
assertEquals(962206200000, Date.UTC(2000, 5, 28, 15, 30, 0, 0));
assertEquals(5679688846670, Date.UTC(2149, 11, 25, 3, 20, 46, 670));


// ----------------------------------------------------------------------------
// Date Prototype
// ----------------------------------------------------------------------------

assert(Date.prototype);
assert(typeof Date.prototype === 'object');


// Thu, 01 Jan 1970 05:00:00 GMT
var d1 = new Date(1970, 0, 1, 0, 0, 0, 0);
var d1u = Date.UTC(1970, 0, 1, 0, 0, 0, 0);

// Mon, 07 May 1990 04:05:00 GMT
var d2 = new Date(1990, 4, 7, 0, 5, 0, 0);
var d2u = Date.UTC(1990, 4, 7, 0, 5, 0, 0);

// Wed, 28 Jun 2000 19:30:00 GMT
var d3 = new Date(2000, 5, 28, 15, 30, 0, 0);
var d3u = Date.UTC(2000, 5, 28, 15, 30, 0, 0);

// Thu, 25 Dec 2149 08:20:46 GMT
var d4 = new Date(2149, 11, 25, 3, 20, 46, 670);
var d4u = Date.UTC(2149, 11, 25, 3, 20, 46, 670);


// Date.prototype.toDateString()
// Date.prototype.toLocaleDateString()

assertEqualsWhenEDT('Thu Jan 01 1970', d1.toDateString());
assertEqualsWhenEDT('Mon May 07 1990', d2.toDateString());
assertEqualsWhenEDT('Wed Jun 28 2000', d3.toDateString());
assertEqualsWhenEDT('Thu Dec 25 2149', d4.toDateString());


// Date.prototype.toISOString()
// Date.prototype.toJSON()

assertEquals('1970-01-01T05:00:00.000Z', d1.toISOString());
assertEquals('1990-05-07T04:05:00.000Z', d2.toISOString());
assertEquals('2000-06-28T19:30:00.000Z', d3.toISOString());
assertEquals('2149-12-25T08:20:46.670Z', d4.toISOString());

assertEquals('1970-01-01T05:00:00.000Z', d1.toJSON());
assertEquals('1990-05-07T04:05:00.000Z', d2.toJSON());
assertEquals('2000-06-28T19:30:00.000Z', d3.toJSON());
assertEquals('2149-12-25T08:20:46.670Z', d4.toJSON());


// Date.prototype.toUTCString()
// Date.prototype.toGMTString()

assertEquals('Thu, 01 Jan 1970 05:00:00 GMT', d1.toUTCString());
assertEquals('Mon, 07 May 1990 04:05:00 GMT', d2.toUTCString());
assertEquals('Wed, 28 Jun 2000 19:30:00 GMT', d3.toUTCString());
assertEquals('Thu, 25 Dec 2149 08:20:46 GMT', d4.toUTCString());

assertEquals('Thu, 01 Jan 1970 05:00:00 GMT', d1.toGMTString());
assertEquals('Mon, 07 May 1990 04:05:00 GMT', d2.toGMTString());
assertEquals('Wed, 28 Jun 2000 19:30:00 GMT', d3.toGMTString());
assertEquals('Thu, 25 Dec 2149 08:20:46 GMT', d4.toGMTString());


// Date.prototype.toTimeString()
// Date.prototype.toLocaleTimeString()

assertEquals('00:00:00 GMT-0500 (EST)', d1.toTimeString());
assertEquals('00:05:00 GMT-0400 (EDT)', d2.toTimeString());
assertEquals('15:30:00 GMT-0400 (EDT)', d3.toTimeString());
assertEquals('03:20:46 GMT-0500 (EST)', d4.toTimeString());


// Date.prototype.toString()
// Date.prototype.toLocaleString()

assertEquals('Thu Jan 01 1970 00:00:00 GMT-0500 (EST)', d1.toString());
assertEquals('Mon May 07 1990 00:05:00 GMT-0400 (EDT)', d2.toString());
assertEquals('Wed Jun 28 2000 15:30:00 GMT-0400 (EDT)', d3.toString());
assertEquals('Thu Dec 25 2149 03:20:46 GMT-0500 (EST)', d4.toString());


// Date.prototype.valueOf()

assertEquals(0, d1u);
assertEquals(642038700000, d2u);
assertEquals(962206200000, d3u);
assertEquals(5679688846670, d4u);

assertEqualsWhenEDT(18000000, d1.valueOf());
assertEqualsWhenEDT(642053100000, d2.valueOf());
assertEqualsWhenEDT(962220600000, d3.valueOf());
assertEqualsWhenEDT(5679706846670, d4.valueOf());


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


var d = new Date(0);


// Date.prototype.setDate(dayValue)
// Date.prototype.setUTCDate(dayValue)

d.setDate(20);
assertEqualsWhenEDT(20, d.getDate());
d.setDate(3);
assertEqualsWhenEDT(3, d.getDate());
d.setDate(0);
assertEqualsWhenEDT(30, d.getDate());
d.setDate(32);
assertEqualsWhenEDT(2, d.getDate());

d.setUTCDate(15);
assertEquals(15, d.getUTCDate());
d.setUTCDate(9);
assertEquals(9, d.getUTCDate());
d.setUTCDate(0);
assertEquals(30, d.getUTCDate());
d.setUTCDate(32);
assertEquals(2, d.getUTCDate());


// Date.prototype.setFullYear(yearValue[, monthValue[, dayValue]])
// Date.prototype.setUTCFullYear(yearValue[, monthValue[, dayValue]])

d.setFullYear(1990);
assertEqualsWhenEDT(1990, d.getFullYear());

d.setFullYear(1700);
assertEqualsWhenEDT(1700, d.getFullYear());

d.setFullYear(2200, 11);
assertEqualsWhenEDT(2200, d.getFullYear());
assertEqualsWhenEDT(11, d.getMonth());

d.setFullYear(1645, 5, 21);
assertEqualsWhenEDT(1645, d.getFullYear());
assertEqualsWhenEDT(5, d.getMonth());
assertEqualsWhenEDT(21, d.getDate());


d.setUTCFullYear(1880);
assertEquals(1880, d.getUTCFullYear());

d.setUTCFullYear(1950);
assertEquals(1950, d.getUTCFullYear());

d.setUTCFullYear(1971, 3);
assertEquals(1971, d.getUTCFullYear());
assertEquals(3, d.getUTCMonth());

d.setUTCFullYear(2050, 10, 30);
assertEquals(2050, d.getUTCFullYear());
assertEquals(10, d.getUTCMonth());
assertEquals(30, d.getUTCDate());


// Date.prototype.setHours(hoursValue[, minutesValue[, secondsValue[, msValue]]])
// Date.prototype.setUTCHours(hoursValue[, minutesValue[, secondsValue[, msValue]]])

d.setHours(22);
assertEqualsWhenEDT(22, d.getHours());

d.setHours(18, 21);
assertEqualsWhenEDT(18, d.getHours());
assertEqualsWhenEDT(21, d.getMinutes());

d.setHours(4, 59, 10);
assertEqualsWhenEDT(4, d.getHours());
assertEqualsWhenEDT(59, d.getMinutes());
assertEqualsWhenEDT(10, d.getSeconds());

d.setHours(0, 12, 40, 300);
assertEqualsWhenEDT(0, d.getHours());
assertEqualsWhenEDT(12, d.getMinutes());
assertEqualsWhenEDT(40, d.getSeconds());
assertEqualsWhenEDT(300, d.getMilliseconds());


d.setUTCHours(22);
assertEquals(22, d.getUTCHours());

d.setUTCHours(18, 21);
assertEquals(18, d.getUTCHours());
assertEquals(21, d.getUTCMinutes());

d.setUTCHours(4, 59, 10);
assertEquals(4, d.getUTCHours());
assertEquals(59, d.getUTCMinutes());
assertEquals(10, d.getUTCSeconds());

d.setUTCHours(0, 12, 40, 300);
assertEquals(0, d.getUTCHours());
assertEquals(12, d.getUTCMinutes());
assertEquals(40, d.getUTCSeconds());
assertEquals(300, d.getUTCMilliseconds());


// Date.prototype.setMilliseconds(msValue)
// Date.prototype.setUTCMilliseconds(msValue)

d.setMilliseconds(984);
assertEquals(984, d.getMilliseconds());

d.setUTCMilliseconds(571);
assertEquals(571, d.getUTCMilliseconds());


// Date.prototype.setMinutes(minutesValue[, secondsValue[, msValue]])
// Date.prototype.setUTCMinutes(minutesValue[, secondsValue[, msValue]])

d.setMinutes(3);
assertEqualsWhenEDT(3, d.getMinutes());

d.setMinutes(5, 21);
assertEqualsWhenEDT(5, d.getMinutes());
assertEquals(21, d.getSeconds());

d.setMinutes(53, 50, 320);
assertEqualsWhenEDT(53, d.getMinutes());
assertEquals(50, d.getSeconds());
assertEquals(320, d.getMilliseconds());


d.setUTCMinutes(3);
assertEquals(3, d.getUTCMinutes());

d.setUTCMinutes(5, 12);
assertEquals(5, d.getUTCMinutes());
assertEquals(12, d.getUTCSeconds());

d.setUTCMinutes(53, 50, 320);
assertEquals(53, d.getUTCMinutes());
assertEquals(50, d.getUTCSeconds());
assertEquals(320, d.getUTCMilliseconds());


// Date.prototype.setMonth(monthValue[, dayValue])
// Date.prototype.setUTCMonth(monthValue[, dayValue])

d.setMonth(11);
assertEqualsWhenEDT(11, d.getMonth());

d.setMonth(5, 12);
assertEqualsWhenEDT(5, d.getMonth());
assertEqualsWhenEDT(12, d.getDate());

d.setUTCMonth(11);
assertEqualsWhenEDT(11, d.getUTCMonth());

d.setUTCMonth(5, 12);
assertEqualsWhenEDT(5, d.getUTCMonth());
assertEqualsWhenEDT(12, d.getUTCDate());


// Date.prototype.setSeconds(secondsValue[, msValue])
// Date.prototype.setUTCSeconds(secondsValue[, msValue])

d.setSeconds(24);
assertEquals(24, d.getSeconds());

d.setSeconds(43, 12);
assertEquals(43, d.getSeconds());
assertEquals(12, d.getMilliseconds());


d.setUTCSeconds(18);
assertEquals(18, d.getUTCSeconds());

d.setUTCSeconds(57, 882);
assertEquals(57, d.getUTCSeconds());
assertEquals(882, d.getUTCMilliseconds());


// Date.prototype.setTime(timeValue)

d.setTime(42);
assertEquals(42, d.valueOf());


// Date.prototype.setYear(yearValue)

d.setYear(96);
assertEquals(1996, d.getFullYear());
d.setYear(1996);
assertEquals(1996, d.getFullYear());
d.setYear(2000);
assertEquals(2000, d.getFullYear());
