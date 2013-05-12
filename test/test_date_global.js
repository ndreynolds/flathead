// test_date_global.js
// -------------------
// Tests for the Date global object.

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};

// Get the TZ prefix, e.g. CEST, EDT, IST
var timezonePrefix = (new Date()).toString().match(/\((\w{2,4})\)/)[1];

var tzSpecificAssertionsSkipped = 0;
var tzSpecificAssertionsPassed = 0;

var assertEqualsWhen = function(tzPrefix, a, b) {
  if (tzPrefix === timezonePrefix) {
    assertEquals(a, b);
    tzSpecificAssertionsPassed++;
  } else {
    tzSpecificAssertionsSkipped++;
  }
};

var test = function(name, f) {
  f();
};

console.log('Current time zone: ' + timezonePrefix);


// ----------------------------------------------------------------------------
// Date Global
// ----------------------------------------------------------------------------

assert(Date);
assertEquals('function', typeof Date);

var now = Date.now();
var d = new Date();
d.setMilliseconds(0);
var t = d.valueOf();


// Constructor

test('Constructor', function() {

  test('new Date(ms)', function() {
    assert((new Date).valueOf() >= now);
    assert((new Date()).valueOf() >= now);
    assert((new Date(0)).valueOf() < now);
    assert((new Date(100)).valueOf() < now);
  });

  test('new Date(dateStr)', function() {
    assertEquals(t, (new Date(d.toString())).valueOf());
    assertEquals(t, (new Date(d.toUTCString())).valueOf());
    assertEquals(t, (new Date(d.toISOString())).valueOf());
  });

  test('new Date(y, m, d, h, m, s, ms)', function() {
    assertEqualsWhen('EDT', 18000000, (new Date(1970, 0, 1, 0, 0, 0, 0)).valueOf());
  });

  // These could fail on a slow system or implementation. Need a better test.
  test('Date()', function() {
    assertEquals((new Date(Date.now())).toString(), Date());
    assertEquals((new Date(Date.now())).toString(), Date(10041));
    assertEquals((new Date(Date.now())).toString(), Date(1970, 0, 1, 0, 0, 0, 0));
  });

});


test('Date.now()', function() {
  assert(Date.now() > 1351474364385);
  assert(Date.now() >= now);
});


// This covers the ES5 spec requirements, but is a far cry from other
// implementations (e.g. V8) that provide a lot more flexibility.
test('Date.parse()', function() {
  assertEquals(t, d.valueOf());
  assertEquals(t, Date.parse(d.toString()));
  assertEquals(t, Date.parse(d.toUTCString()));
  assertEquals(t, Date.parse(d.toISOString()));
});


test('Date.UTC()', function() {
  assertEquals(0, Date.UTC(1970, 0, 1, 0, 0, 0, 0));
  assertEquals(642038700000, Date.UTC(1990, 4, 7, 0, 5, 0, 0));
  assertEquals(962206200000, Date.UTC(2000, 5, 28, 15, 30, 0, 0));
  assertEquals(5679688846670, Date.UTC(2149, 11, 25, 3, 20, 46, 670));
});


// ----------------------------------------------------------------------------
// Date Prototype
// ----------------------------------------------------------------------------

assert(Date.prototype);
assert(typeof Date.prototype === 'object');


// Thu, 01 Jan 1970 05:00:00 GMT
var d1 = new Date(1970, 0, 1, 0, 0, 0, 0);
var d1u = new Date(Date.UTC(1970, 0, 1, 0, 0, 0, 0));

// Mon, 07 May 1990 04:05:00 GMT
var d2 = new Date(1990, 4, 7, 0, 5, 0, 0);
var d2u = new Date(Date.UTC(1990, 4, 7, 0, 5, 0, 0));

// Wed, 28 Jun 2000 19:30:00 GMT
var d3 = new Date(2000, 5, 28, 15, 30, 0, 0);
var d3u = new Date(Date.UTC(2000, 5, 28, 15, 30, 0, 0));

// Thu, 25 Dec 2149 08:20:46 GMT
var d4 = new Date(2149, 11, 25, 3, 20, 46, 670);
var d4u = new Date(Date.UTC(2149, 11, 25, 3, 20, 46, 670));


test('Date#toDateString()', function() {
  assertEquals('Thu Jan 01 1970', d1.toDateString());
  assertEquals('Mon May 07 1990', d2.toDateString());
  assertEquals('Wed Jun 28 2000', d3.toDateString());
  assertEquals('Thu Dec 25 2149', d4.toDateString());
});

test('Date#toISOString()', function() {
  assertEqualsWhen('EDT', '1970-01-01T05:00:00.000Z', d1.toISOString());
  assertEqualsWhen('EDT', '1990-05-07T04:05:00.000Z', d2.toISOString());
  assertEqualsWhen('EDT', '2000-06-28T19:30:00.000Z', d3.toISOString());
  assertEqualsWhen('EDT', '2149-12-25T08:20:46.670Z', d4.toISOString());

  assertEquals('1970-01-01T00:00:00.000Z', d1u.toISOString());
  assertEquals('1990-05-07T00:05:00.000Z', d2u.toISOString());
  assertEquals('2000-06-28T15:30:00.000Z', d3u.toISOString());
  assertEquals('2149-12-25T03:20:46.670Z', d4u.toISOString());
});

test('Date#toJSON()', function() {
  assertEqualsWhen('EDT', '1970-01-01T05:00:00.000Z', d1.toJSON());
  assertEqualsWhen('EDT', '1990-05-07T04:05:00.000Z', d2.toJSON());
  assertEqualsWhen('EDT', '2000-06-28T19:30:00.000Z', d3.toJSON());
  assertEqualsWhen('EDT', '2149-12-25T08:20:46.670Z', d4.toJSON());

  assertEquals('1970-01-01T00:00:00.000Z', d1u.toJSON());
  assertEquals('1990-05-07T00:05:00.000Z', d2u.toJSON());
  assertEquals('2000-06-28T15:30:00.000Z', d3u.toJSON());
  assertEquals('2149-12-25T03:20:46.670Z', d4u.toJSON());
});

test('Date#toUTCString()', function() {
  assertEqualsWhen('EDT', 'Thu, 01 Jan 1970 05:00:00 GMT', d1.toUTCString());
  assertEqualsWhen('EDT', 'Mon, 07 May 1990 04:05:00 GMT', d2.toUTCString());
  assertEqualsWhen('EDT', 'Wed, 28 Jun 2000 19:30:00 GMT', d3.toUTCString());
  assertEqualsWhen('EDT', 'Thu, 25 Dec 2149 08:20:46 GMT', d4.toUTCString());

  assertEqualsWhen('CEST', 'Wed, 31 Dec 1969 23:00:00 GMT', d1.toUTCString());
  assertEqualsWhen('CEST', 'Sun, 06 May 1990 22:05:00 GMT', d2.toUTCString());
  assertEqualsWhen('CEST', 'Wed, 28 Jun 2000 13:30:00 GMT', d3.toUTCString());
  assertEqualsWhen('CEST', 'Thu, 25 Dec 2149 02:20:46 GMT', d4.toUTCString());

  assertEqualsWhen('IST', 'Wed, 31 Dec 1969 18:30:00 GMT', d1.toUTCString());
  assertEqualsWhen('IST', 'Sun, 06 May 1990 18:35:00 GMT', d2.toUTCString());
  assertEqualsWhen('IST', 'Wed, 28 Jun 2000 10:00:00 GMT', d3.toUTCString());
  assertEqualsWhen('IST', 'Wed, 24 Dec 2149 21:50:46 GMT', d4.toUTCString());

  assertEquals('Thu, 01 Jan 1970 00:00:00 GMT', d1u.toUTCString());
  assertEquals('Mon, 07 May 1990 00:05:00 GMT', d2u.toUTCString());
  assertEquals('Wed, 28 Jun 2000 15:30:00 GMT', d3u.toUTCString());
  assertEquals('Thu, 25 Dec 2149 03:20:46 GMT', d4u.toUTCString());
});

test('Date#toGMTString()', function() {
  assertEqualsWhen('EDT', 'Thu, 01 Jan 1970 05:00:00 GMT', d1.toGMTString());
  assertEqualsWhen('EDT', 'Mon, 07 May 1990 04:05:00 GMT', d2.toGMTString());
  assertEqualsWhen('EDT', 'Wed, 28 Jun 2000 19:30:00 GMT', d3.toGMTString());
  assertEqualsWhen('EDT', 'Thu, 25 Dec 2149 08:20:46 GMT', d4.toGMTString());

  assertEqualsWhen('CEST', 'Wed, 31 Dec 1969 23:00:00 GMT', d1.toGMTString());
  assertEqualsWhen('CEST', 'Sun, 06 May 1990 22:05:00 GMT', d2.toGMTString());
  assertEqualsWhen('CEST', 'Wed, 28 Jun 2000 13:30:00 GMT', d3.toGMTString());
  assertEqualsWhen('CEST', 'Thu, 25 Dec 2149 02:20:46 GMT', d4.toGMTString());

  assertEqualsWhen('IST', 'Wed, 31 Dec 1969 18:30:00 GMT', d1.toGMTString());
  assertEqualsWhen('IST', 'Sun, 06 May 1990 18:35:00 GMT', d2.toGMTString());
  assertEqualsWhen('IST', 'Wed, 28 Jun 2000 10:00:00 GMT', d3.toGMTString());
  assertEqualsWhen('IST', 'Wed, 24 Dec 2149 21:50:46 GMT', d4.toGMTString());

  assertEquals('Thu, 01 Jan 1970 00:00:00 GMT', d1u.toGMTString());
  assertEquals('Mon, 07 May 1990 00:05:00 GMT', d2u.toGMTString());
  assertEquals('Wed, 28 Jun 2000 15:30:00 GMT', d3u.toGMTString());
  assertEquals('Thu, 25 Dec 2149 03:20:46 GMT', d4u.toGMTString());
});

test('Date#toTimeString()', function() {
  assertEqualsWhen('EDT', '00:00:00 GMT-0500 (EST)', d1.toTimeString());
  assertEqualsWhen('EDT', '00:05:00 GMT-0400 (EDT)', d2.toTimeString());
  assertEqualsWhen('EDT', '15:30:00 GMT-0400 (EDT)', d3.toTimeString());
  assertEqualsWhen('EDT', '03:20:46 GMT-0500 (EST)', d4.toTimeString());

  assertEqualsWhen('CEST', '00:00:00 GMT+0100 (CET)',  d1.toTimeString());
  assertEqualsWhen('CEST', '00:05:00 GMT+0200 (CEST)', d2.toTimeString());
  assertEqualsWhen('CEST', '15:30:00 GMT+0200 (CEST)', d3.toTimeString());
  assertEqualsWhen('CEST', '03:20:46 GMT+0100 (CET)',  d4.toTimeString());

  assertEqualsWhen('IST', '00:00:00 GMT+0530 (IST)', d1.toTimeString());
  assertEqualsWhen('IST', '00:05:00 GMT+0530 (IST)', d2.toTimeString());
  assertEqualsWhen('IST', '15:30:00 GMT+0530 (IST)', d3.toTimeString());
  assertEqualsWhen('IST', '03:20:46 GMT+0530 (IST)', d4.toTimeString());
});

test('Date#toString()', function() {
  assertEqualsWhen('EDT', 'Thu Jan 01 1970 00:00:00 GMT-0500 (EST)', d1.toString());
  assertEqualsWhen('EDT', 'Mon May 07 1990 00:05:00 GMT-0400 (EDT)', d2.toString());
  assertEqualsWhen('EDT', 'Wed Jun 28 2000 15:30:00 GMT-0400 (EDT)', d3.toString());
  assertEqualsWhen('EDT', 'Thu Dec 25 2149 03:20:46 GMT-0500 (EST)', d4.toString());

  assertEqualsWhen('CEST', 'Thu Jan 01 1970 00:00:00 GMT+0100 (CET)',  d1.toString());
  assertEqualsWhen('CEST', 'Mon May 07 1990 00:05:00 GMT+0200 (CEST)', d2.toString());
  assertEqualsWhen('CEST', 'Wed Jun 28 2000 15:30:00 GMT+0200 (CEST)', d3.toString());
  assertEqualsWhen('CEST', 'Thu Dec 25 2149 03:20:46 GMT+0100 (CET)',  d4.toString());

  assertEqualsWhen('IST', 'Thu Jan 01 1970 00:00:00 GMT+0530 (IST)', d1.toString());
  assertEqualsWhen('IST', 'Mon May 07 1990 00:05:00 GMT+0530 (IST)', d2.toString());
  assertEqualsWhen('IST', 'Wed Jun 28 2000 15:30:00 GMT+0530 (IST)', d3.toString());
  assertEqualsWhen('IST', 'Thu Dec 25 2149 03:20:46 GMT+0530 (IST)', d4.toString());
});

test('Date#valueOf()', function() {
  assertEqualsWhen('EDT', 18000000,      d1.valueOf());
  assertEqualsWhen('EDT', 642053100000,  d2.valueOf());
  assertEqualsWhen('EDT', 962220600000,  d3.valueOf());
  assertEqualsWhen('EDT', 5679706846670, d4.valueOf());

  assertEquals(0,             d1u.valueOf());
  assertEquals(642038700000,  d2u.valueOf());
  assertEquals(962206200000,  d3u.valueOf());
  assertEquals(5679688846670, d4u.valueOf());
});

test('Date#getDate()', function() {
  assertEquals(1,  d1.getDate());
  assertEquals(7,  d2.getDate());
  assertEquals(28, d3.getDate());
  assertEquals(25, d4.getDate());
});

test('Date#getUTCDate()', function() {
  assertEqualsWhen('EDT', 1,  d1.getUTCDate());
  assertEqualsWhen('EDT', 7,  d2.getUTCDate());
  assertEqualsWhen('EDT', 28, d3.getUTCDate());
  assertEqualsWhen('EDT', 25, d4.getUTCDate());

  assertEqualsWhen('CEST', 31, d1.getUTCDate());
  assertEqualsWhen('CEST', 6,  d2.getUTCDate());
  assertEqualsWhen('CEST', 28, d3.getUTCDate());
  assertEqualsWhen('CEST', 25, d4.getUTCDate());

  assertEqualsWhen('IST', 31, d1.getUTCDate());
  assertEqualsWhen('IST', 6,  d2.getUTCDate());
  assertEqualsWhen('IST', 28, d3.getUTCDate());
  assertEqualsWhen('IST', 24, d4.getUTCDate());

  assertEquals(1,  d1u.getUTCDate());
  assertEquals(7,  d2u.getUTCDate());
  assertEquals(28, d3u.getUTCDate());
  assertEquals(25, d4u.getUTCDate());
});

test('Date#getDay()', function() {
  assertEqualsWhen('EDT', 4, d1.getDay());
  assertEqualsWhen('EDT', 1, d2.getDay());
  assertEqualsWhen('EDT', 3, d3.getDay());
  assertEqualsWhen('EDT', 4, d4.getDay());

  assertEqualsWhen('CEST', 4, d1.getDay());
  assertEqualsWhen('CEST', 1, d2.getDay());
  assertEqualsWhen('CEST', 3, d3.getDay());
  assertEqualsWhen('CEST', 4, d4.getDay());

  assertEqualsWhen('IST', 4, d1.getDay());
  assertEqualsWhen('IST', 1, d2.getDay());
  assertEqualsWhen('IST', 3, d3.getDay());
  assertEqualsWhen('IST', 4, d4.getDay());
});

test('Date#getUTCDay()', function() {
  assertEqualsWhen('EDT', 4, d1.getUTCDay());
  assertEqualsWhen('EDT', 1, d2.getUTCDay());
  assertEqualsWhen('EDT', 3, d3.getUTCDay());
  assertEqualsWhen('EDT', 4, d4.getUTCDay());

  assertEquals(4, d1u.getUTCDay());
  assertEquals(1, d2u.getUTCDay());
  assertEquals(3, d3u.getUTCDay());
  assertEquals(4, d4u.getUTCDay());
});

test('Date#getFullYear()', function() {
  assertEqualsWhen('EDT', 1970, d1.getFullYear());
  assertEqualsWhen('EDT', 1990, d2.getFullYear());
  assertEqualsWhen('EDT', 2000, d3.getFullYear());
  assertEqualsWhen('EDT', 2149, d4.getFullYear());

  assertEqualsWhen('CEST', 1970, d1.getFullYear());
  assertEqualsWhen('CEST', 1990, d2.getFullYear());
  assertEqualsWhen('CEST', 2000, d3.getFullYear());
  assertEqualsWhen('CEST', 2149, d4.getFullYear());

  assertEqualsWhen('IST', 1970, d1.getFullYear());
  assertEqualsWhen('IST', 1990, d2.getFullYear());
  assertEqualsWhen('IST', 2000, d3.getFullYear());
  assertEqualsWhen('IST', 2149, d4.getFullYear());
});

test('Date#getUTCFullYear()', function() {
  assertEqualsWhen('EDT', 1970, d1.getUTCFullYear());
  assertEqualsWhen('EDT', 1990, d2.getUTCFullYear());
  assertEqualsWhen('EDT', 2000, d3.getUTCFullYear());
  assertEqualsWhen('EDT', 2149, d4.getUTCFullYear());

  assertEquals(1970, d1u.getUTCFullYear());
  assertEquals(1990, d2u.getUTCFullYear());
  assertEquals(2000, d3u.getUTCFullYear());
  assertEquals(2149, d4u.getUTCFullYear());
});

test('Date#getHours()', function() {
  assertEqualsWhen('EDT', 0,  d1.getHours());
  assertEqualsWhen('EDT', 0,  d2.getHours());
  assertEqualsWhen('EDT', 15, d3.getHours());
  assertEqualsWhen('EDT', 3,  d4.getHours());

  assertEqualsWhen('CEST', 0,  d1.getHours());
  assertEqualsWhen('CEST', 0,  d2.getHours());
  assertEqualsWhen('CEST', 15, d3.getHours());
  assertEqualsWhen('CEST', 3,  d4.getHours());

  assertEqualsWhen('IST', 0,  d1.getHours());
  assertEqualsWhen('IST', 0,  d2.getHours());
  assertEqualsWhen('IST', 15, d3.getHours());
  assertEqualsWhen('IST', 3,  d4.getHours());
});

test('Date#getUTCHours()', function() {
  assertEqualsWhen('EDT', 5,  d1.getUTCHours());
  assertEqualsWhen('EDT', 4,  d2.getUTCHours());
  assertEqualsWhen('EDT', 19, d3.getUTCHours());
  assertEqualsWhen('EDT', 8,  d4.getUTCHours());

  assertEquals(0,  d1u.getUTCHours());
  assertEquals(0,  d2u.getUTCHours());
  assertEquals(15, d3u.getUTCHours());
  assertEquals(3,  d4u.getUTCHours());
});

test('Date#getMilliseconds()', function() {
  assertEquals(0,   d1.getMilliseconds());
  assertEquals(0,   d2.getMilliseconds());
  assertEquals(0,   d3.getMilliseconds());
  assertEquals(670, d4.getMilliseconds());

  assertEquals(0,   d1u.getMilliseconds());
  assertEquals(0,   d2u.getMilliseconds());
  assertEquals(0,   d3u.getMilliseconds());
  assertEquals(670, d4u.getMilliseconds());
});

test('Date#getUTCMilliseconds()', function() {
  assertEquals(0,   d1.getUTCMilliseconds());
  assertEquals(0,   d2.getUTCMilliseconds());
  assertEquals(0,   d3.getUTCMilliseconds());
  assertEquals(670, d4.getUTCMilliseconds());

  assertEquals(0,   d1u.getUTCMilliseconds());
  assertEquals(0,   d2u.getUTCMilliseconds());
  assertEquals(0,   d3u.getUTCMilliseconds());
  assertEquals(670, d4u.getUTCMilliseconds());
});

test('Date#getMinutes()', function() {
  assertEqualsWhen('EDT', 0,  d1.getMinutes());
  assertEqualsWhen('EDT', 5,  d2.getMinutes());
  assertEqualsWhen('EDT', 30, d3.getMinutes());
  assertEqualsWhen('EDT', 20, d4.getMinutes());

  assertEqualsWhen('CEST', 0,  d1.getMinutes());
  assertEqualsWhen('CEST', 5,  d2.getMinutes());
  assertEqualsWhen('CEST', 30, d3.getMinutes());
  assertEqualsWhen('CEST', 20, d4.getMinutes());

  assertEqualsWhen('IST', 0,  d1.getMinutes());
  assertEqualsWhen('IST', 5,  d2.getMinutes());
  assertEqualsWhen('IST', 30, d3.getMinutes());
  assertEqualsWhen('IST', 20, d4.getMinutes());
});

test('Date#getUTCMinutes()', function() {
  assertEqualsWhen('EDT', 0,  d1.getUTCMinutes());
  assertEqualsWhen('EDT', 5,  d2.getUTCMinutes());
  assertEqualsWhen('EDT', 30, d3.getUTCMinutes());
  assertEqualsWhen('EDT', 20, d4.getUTCMinutes());

  assertEquals(0,  d1u.getUTCMinutes());
  assertEquals(5,  d2u.getUTCMinutes());
  assertEquals(30, d3u.getUTCMinutes());
  assertEquals(20, d4u.getUTCMinutes());
});

test('Date#getMonth()', function() {
  assertEqualsWhen('EDT', 0,  d1.getMonth());
  assertEqualsWhen('EDT', 4,  d2.getMonth());
  assertEqualsWhen('EDT', 5,  d3.getMonth());
  assertEqualsWhen('EDT', 11, d4.getMonth());

  assertEqualsWhen('CEST', 0,  d1.getMonth());
  assertEqualsWhen('CEST', 4,  d2.getMonth());
  assertEqualsWhen('CEST', 5,  d3.getMonth());
  assertEqualsWhen('CEST', 11, d4.getMonth());

  assertEqualsWhen('IST', 0,  d1.getMonth());
  assertEqualsWhen('IST', 4,  d2.getMonth());
  assertEqualsWhen('IST', 5,  d3.getMonth());
  assertEqualsWhen('IST', 11, d4.getMonth());
});

test('Date#getUTCMonth()', function() {
  assertEqualsWhen('EDT', 0,  d1.getUTCMonth());
  assertEqualsWhen('EDT', 4,  d2.getUTCMonth());
  assertEqualsWhen('EDT', 5,  d3.getUTCMonth());
  assertEqualsWhen('EDT', 11, d4.getUTCMonth());

  assertEquals(0,  d1u.getUTCMonth());
  assertEquals(4,  d2u.getUTCMonth());
  assertEquals(5,  d3u.getUTCMonth());
  assertEquals(11, d4u.getUTCMonth());
});

test('Date#getSeconds()', function() {
  assertEquals(0,  d1.getSeconds());
  assertEquals(0,  d2.getSeconds());
  assertEquals(0,  d3.getSeconds());
  assertEquals(46, d4.getSeconds());

  assertEquals(0,  d1u.getSeconds());
  assertEquals(0,  d2u.getSeconds());
  assertEquals(0,  d3u.getSeconds());
  assertEquals(46, d4u.getSeconds());
});

test('Date#getUTCSeconds()', function() {
  assertEquals(0,  d1.getUTCSeconds());
  assertEquals(0,  d2.getUTCSeconds());
  assertEquals(0,  d3.getUTCSeconds());
  assertEquals(46, d4.getUTCSeconds());

  assertEquals(0,  d1u.getUTCSeconds());
  assertEquals(0,  d2u.getUTCSeconds());
  assertEquals(0,  d3u.getUTCSeconds());
  assertEquals(46, d4u.getUTCSeconds());
});

test('Date#getYear()', function() {
  assertEquals(70,  d1.getYear());
  assertEquals(90,  d2.getYear());
  assertEquals(100, d3.getYear());
  assertEquals(249, d4.getYear());
});


var d = new Date(0);
var du = new Date(Date.UTC(1970, 0, 1, 0, 0, 0, 0));


test('Date#setDate(dayValue)', function() {
  d.setDate(20);
  assertEqualsWhen('EDT', 20, d.getDate());
  assertEqualsWhen('CEST', 20, d.getDate());
  assertEqualsWhen('IST', 20, d.getDate());

  d.setDate(3);
  assertEqualsWhen('EDT', 3, d.getDate());
  assertEqualsWhen('CEST', 3, d.getDate());
  assertEqualsWhen('IST', 3, d.getDate());

  d.setDate(0);
  assertEqualsWhen('EDT', 30, d.getDate());
  assertEqualsWhen('CEST', 31, d.getDate());
  assertEqualsWhen('IST', 31, d.getDate());

  d.setDate(32);
  assertEqualsWhen('EDT', 2, d.getDate());
  assertEqualsWhen('CEST', 1, d.getDate());
  assertEqualsWhen('IST', 1, d.getDate());
});

test('Date#setUTCDate(dayValue)', function() {
  d.setUTCDate(15);
  assertEqualsWhen('EDT', 15, d.getUTCDate());

  du.setUTCDate(15);
  assertEquals(15, du.getUTCDate());

  d.setUTCDate(9);
  assertEqualsWhen('EDT', 9, d.getUTCDate());

  du.setUTCDate(9);
  assertEquals(9, du.getUTCDate());

  d.setUTCDate(0);
  assertEqualsWhen('EDT', 30, d.getUTCDate());

  du.setUTCDate(0);
  assertEquals(31, du.getUTCDate());

  d.setUTCDate(32);
  assertEqualsWhen('EDT', 2, d.getUTCDate());

  du.setUTCDate(32);
  assertEquals(1, du.getUTCDate());
});

test('Date#setFullYear(y[, m[, d]])', function() {
  d.setFullYear(1990);
  assertEquals(1990, d.getFullYear());

  d.setFullYear(1700);
  assertEquals(1700, d.getFullYear());

  d.setFullYear(2200, 11);
  assertEquals(2200, d.getFullYear());
  assertEquals(11, d.getMonth());

  d.setFullYear(1645, 5, 21);
  assertEquals(1645, d.getFullYear());
  assertEquals(5, d.getMonth());
  assertEquals(21, d.getDate());
});

test('Date#setUTCFullYear(y[, m[, d]])', function() {
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
});

test('Date#setHours(h[, m[, s[, ms]]])', function() {
  d.setHours(22);
  assertEqualsWhen('EDT', 22, d.getHours());

  d.setHours(18, 21);
  assertEquals(18, d.getHours());
  assertEquals(21, d.getMinutes());

  d.setHours(4, 59, 10);
  assertEquals(4, d.getHours());
  assertEquals(59, d.getMinutes());
  assertEquals(10, d.getSeconds());

  d.setHours(0, 12, 40, 300);
  assertEquals(0, d.getHours());
  assertEquals(12, d.getMinutes());
  assertEquals(40, d.getSeconds());
  assertEquals(300, d.getMilliseconds());
});

test('Date#setUTCHours(h[, m[, s[, ms]]])', function() {
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
});

test('Date#setMilliseconds(ms)', function() {
  d.setMilliseconds(984);
  assertEquals(984, d.getMilliseconds());
});

test('Date#setUTCMilliseconds(ms)', function() {
  d.setUTCMilliseconds(571);
  assertEquals(571, d.getUTCMilliseconds());
});

test('Date#setMinutes(m[, s[, ms]])', function() {
  d.setMinutes(3);
  assertEquals(3, d.getMinutes());

  d.setMinutes(5, 21);
  assertEquals(5, d.getMinutes());
  assertEquals(21, d.getSeconds());

  d.setMinutes(53, 50, 320);
  assertEquals(53, d.getMinutes());
  assertEquals(50, d.getSeconds());
  assertEquals(320, d.getMilliseconds());
});

test('Date#setUTCMinutes(m[, s[, ms]])', function() {
  d.setUTCMinutes(3);
  assertEquals(3, d.getUTCMinutes());

  d.setUTCMinutes(5, 12);
  assertEquals(5, d.getUTCMinutes());
  assertEquals(12, d.getUTCSeconds());

  d.setUTCMinutes(53, 50, 320);
  assertEquals(53, d.getUTCMinutes());
  assertEquals(50, d.getUTCSeconds());
  assertEquals(320, d.getUTCMilliseconds());
});

test('Date#setMonth(m[, d])', function() {
  d.setMonth(11);
  assertEquals(11, d.getMonth());

  d.setMonth(5, 12);
  assertEquals(5, d.getMonth());
  assertEquals(12, d.getDate());
});

test('Date#setUTCMonth(m[, d])', function() {
  d.setUTCMonth(11);
  assertEquals(11, d.getUTCMonth());

  d.setUTCMonth(5, 12);
  assertEquals(5, d.getUTCMonth());
  assertEquals(12, d.getUTCDate());
});

test('Date#setSeconds(s[, ms])', function() {
  d.setSeconds(24);
  assertEquals(24, d.getSeconds());

  d.setSeconds(43, 12);
  assertEquals(43, d.getSeconds());
  assertEquals(12, d.getMilliseconds());
});

test('Date#setUTCSeconds(s[, ms])', function() {
  d.setUTCSeconds(18);
  assertEquals(18, d.getUTCSeconds());

  d.setUTCSeconds(57, 882);
  assertEquals(57, d.getUTCSeconds());
  assertEquals(882, d.getUTCMilliseconds());
});

test('Date#setTime(timeValue)', function() {
  d.setTime(42);
  assertEquals(42, d.valueOf());
});

test('Date#setYear(yearValue)', function() {
  d.setYear(96);
  assertEquals(1996, d.getFullYear());

  du.setYear(96);
  assertEquals(1996, du.getFullYear());

  d.setYear(1996);
  assertEquals(1996, d.getFullYear());

  du.setYear(1996);
  assertEquals(1996, du.getFullYear());

  d.setYear(2000);
  assertEquals(2000, d.getFullYear());

  du.setYear(2000);
  assertEquals(2000, du.getFullYear());
});

console.log(tzSpecificAssertionsSkipped + ' tz-specific assertions skipped');
console.log(tzSpecificAssertionsPassed + ' tz-specific assertions passed');
