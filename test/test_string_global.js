// test_string_global.js
// ---------------------
// Tests for the String global object.

var assert = console.assert;


// ----------------------------------------------------------------------------
// String Global
// ----------------------------------------------------------------------------

assert(String);
assert(typeof String === 'function');

assert(String.fromCharCode);


// ----------------------------------------------------------------------------
// String Prototype
// ----------------------------------------------------------------------------

assert(String.prototype);


// String.prototype.charAt(index)

var s = "abc";
assert(s.charAt() === "a");
assert(s.charAt(0) === "a");
assert(s.charAt(1) === "b");
assert(s.charAt(2) === "c");
assert(s.charAt(999) === "");
assert(s.charAt(-1) === "");


// String.prototype.charCodeAt(index)

assert(s.charCodeAt);


// String.prototype.concat(string2, string3[, ..., stringN])

s = "Hello, ";
assert(s.concat("Kevin", " have a nice day.") === "Hello, Kevin have a nice day.");
assert(s === "Hello, ");


// String.prototype.indexOf(searchValue[, fromIndex])

s = "Blue Whale";
assert(s.indexOf("Blue") === 0);
assert(s.indexOf("Blute") === -1);
assert(s.indexOf("Whale", 0) === 5);
assert(s.indexOf("Whale", 5) === 5);
assert(s.indexOf("", 9) === 9);
assert(s.indexOf("", 10) === 10);
assert(s.indexOf("", 11) === 10);
assert(s.indexOf("blue", 11) === -1);


// String.prototype.lastIndexOf(searchValue[, fromIndex])

s = "canal";
assert(s.lastIndexOf("a") === 3);
assert(s.lastIndexOf("a", 2) === 1);
assert(s.lastIndexOf("a", 0) === -1);
assert(s.lastIndexOf("x") === -1);
assert(s.lastIndexOf("A") === -1);

s = "test test test";
assert(s.lastIndexOf("test", Infinity) === 10);
assert(s.lastIndexOf("test", s.length * 2) === 10);
assert(s.lastIndexOf("test", 15) === 10);
assert(s.lastIndexOf("test", 14) === 10);
assert(s.lastIndexOf("test", 10) === 10);
assert(s.lastIndexOf("test", 9) === 5);
assert(s.lastIndexOf("test", 6) === 5);
assert(s.lastIndexOf("test", 5) === 5);
assert(s.lastIndexOf("test", 4) === 0);
assert(s.lastIndexOf("test", 0) === 0);
assert(s.lastIndexOf("test", -1) === 0);
assert(s.lastIndexOf("test", -s.length) === 0);
assert(s.lastIndexOf("test", -Infinity) === 0);
assert(s.lastIndexOf("test") === 10);
assert(s.lastIndexOf("notpresent") === -1);
assert(s.lastIndexOf() === -1);
assert(s.lastIndexOf("test", "not a number") === 10);


// String.prototype.localeCompare(compareString)

s = "abcd";
assert(s.localeCompare("abcd") === 0);
assert(s.localeCompare("abc") > 0);
assert(s.localeCompare("abcde") < 0);
assert(s.localeCompare("abcx") < 0);
assert(s.localeCompare("abtd") < 0);
assert(s.localeCompare("aacd") > 0);


// String.prototype.match(regexp)
// String.prototype.replace(regexp|substr, newSubStr|function[, flags])
// String.prototype.search(regexp)


// String.prototype.slice(beginSlice[, endSlice])

s = "slice me";
assert(s.slice(0) === "slice me");
assert(s.slice(2) === "ice me");
assert(s.slice(0, 2) === "sl");
assert(s.slice(0, 4) === "slic");
assert(s.slice(2, 4) === "ic");
assert(s.slice(0, 24) === "slice me");
assert(s.slice(2, 100) === "ice me");
assert(s.slice(0, -3) === "slice");
assert(s.slice(0, -500) === "");
assert(s.slice(-500, -500) === "");
assert(s.slice(-500, 2) === "sl");


// String.prototype.split([separator][, limit])

var arr;
s = "abc,def,ghi";

arr = s.split();
assert(arr[0] === "abc,def,ghi");
assert(arr.length === 1);

arr = s.split(",", 0);
assert(arr.length === 0);

arr = s.split(",");
assert(arr.length === 3);
assert(arr[0] === "abc");
assert(arr[1] === "def");
assert(arr[2] === "ghi");

arr = s.split(",", 2);
assert(arr.length === 2);
assert(arr[0] === "abc");
assert(arr[1] === "def");


// String.prototype.substr(start[, length])

s = "abcdefghij";
assert(s.substr(1,2) === "bc");
assert(s.substr(-3,2) === "hi");
assert(s.substr(-3) === "hij");
assert(s.substr(1) === "bcdefghij");
assert(s.substr(-20,2) === "ab");
assert(s.substr(20,2) === "");


// String.prototype.substring(start[, end])

s = "JavaScript";
assert(s.substring(0, 3) === "Jav");
assert(s.substring(3, 0) === "Jav");
assert(s.substring(4, 7) === "Scr");
assert(s.substring(7, 4) === "Scr");
assert(s.substring(0, 4) === "Java");
assert(s.substring(0, 10) === "JavaScript");
assert(s.substring(10, 0) === "JavaScript");
assert(s.substring(1000, 1000) === "");
assert(s.substring(-999, 1000) === "JavaScript");
assert(s.substring(-999, -999) === "");


// String.prototype.toLocaleLowerCase()
// String.prototype.toLowerCase()

s = "ALPHABET";
assert(s.toLocaleLowerCase() === "alphabet");
assert(s.toLowerCase() === "alphabet");


// String.prototype.toLocaleUpperCase()
// String.prototype.toUpperCase()

s = "alphabet";
assert(s.toLocaleUpperCase() === "ALPHABET");
assert(s.toUpperCase() === "ALPHABET");


// String.prototype.toString()
// String.prototype.valueOf()

s = "I'm a string";
assert(s.toString() === "I'm a string");
assert(s.valueOf() === "I'm a string");


// String.prototype.trim()
// String.prototype.trimLeft()
// String.prototype.trimRight()

s = "    lots of whitespace        ";
assert(s.trim() === "lots of whitespace");
assert(s.trimLeft() === "lots of whitespace        ");
assert(s.trimRight() === "    lots of whitespace");
