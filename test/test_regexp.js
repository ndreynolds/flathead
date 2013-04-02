// test_regexp.js
// --------------
// Verify that these RegExp literals can all be parsed.

/*this comment shouldn't cause problems*/
/*orthisone/ */

var re;

re = /abc/;
re = / abc /;
re = /\/abc/;
re = /\\/.exec("/");
re = /ab\/c/;
re = /a{1,3}/;
re = /\\\\/;
re = /abc/igm;
re = /\\/g;
re = /^0/;
re = /(.)^/i;
re = /./g;
re = /x(?:...|(...))\1x/i;
re = /\\|'|\r|\n|\t|\u2028|\u2029/g;
re = /<%([\s\S]+?)%>/g;
re = /\(\d{2}.\w{3}.\d{4}\)[\s\-.,]{0,3}\b\w+\b/i;
re = /^(.+?)(\d+)\.(\S+)$/;

if (true) /regex/.exec('regex');

// Division should not be confused with a RegExp literal.
var x = 12 / 3 / 2;
console.assert(x === 2);

var a = 12, b = 2;
a++ / b;

// FIXME: This final example doesn't work because when the preceding token is a
// ++, we assume that it is a division, but it could be either.
//
// ++/foo/.abc;
