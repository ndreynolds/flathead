// test_operators.js
// -----------------

var assert = console.assert;
var assertEquals = function(a, b) {
  assert(a === b);
};

// ------------------------------------------------------------------
// UNARY PREFIX
// ------------------------------------------------------------------

var a = 0;
assert(++a === 1);

var b = 2;
assert(--b === 1);

var numString = "3.14";

assert(+numString === 3.14);
assert(-numString === -3.14);
assert(-Infinity !== Infinity);
assert(!numString === false);
assert(!!numString === true);
assert(!false === true);
assert(isNaN(+{}));

var result = ++numString;

// Handle IEEE precision quirks
assert(result >= 4.14 && result <= 4.14 + 0.0000001);

var alphaString = "abc";
assert(isNaN(++alphaString));

// typeof operator
assert(typeof "hello" === "string");
assert(typeof 42 === "number");
assert(typeof true === "boolean");
assert(typeof null === "object");
assert(typeof undefined === "undefined");
assert(typeof {a: 12} === "object");
assert(typeof [1, 2, 3] === "object");
assert(typeof function() { return 42; } === "function");
assert(typeof '1' + 12 === 'string12');

// void operator
assert(void 'boom' === undefined);
assert(void 12 === undefined);

var counter = 0;

assert(void ++counter === undefined);
assertEquals(counter, 1);
assert(void (counter *= 4) === undefined);
assertEquals(counter, 4);

// delete operator
// TODO


// ------------------------------------------------------------------
// UNARY POSTFIX
// ------------------------------------------------------------------

var c = 0;
assert(c++ === 0);
assert(c === 1);

var d = 2;
assert(d-- === 2);
assert(d === 1);


// ------------------------------------------------------------------
// BINARY
// ------------------------------------------------------------------


// Equality

assert(true == true);
assert(true != false);
assert(null == undefined);
assert(null !== undefined);
assert(NaN != NaN && NaN !== NaN);
assert(Infinity == Infinity && Infinity === Infinity);
assert(2 == '2' && 2 !== '2');

assert('' != '0');
assert(0 == '');
assert(0 == '0');
assert(false != 'false');
assert(false == '0');
assert(false != undefined);
assert(false != null);

var obj = {};
var differentObj = {};
assert(obj == obj && obj === obj);
assert(obj != differentObj && obj !== differentObj);


// Concatenation

assert(("Hello" + "Goodbye") === "HelloGoodbye");
assert(("Hello" + 12) === "Hello12");
assert(("Hello" + undefined) === "Helloundefined");
assert(("Hello" + null) === "Hellonull");
assert(("Hello" + true) === "Hellotrue");
assert(("Hello" + false) === "Hellofalse");


// Arithmetic

assert((2 + 2) === 4);
assert((3 - 7) === -4);
assert((10 * 16) === 160);
assert((9 / 3) === 3);
assert((10 % 7) === 3);


// Logical

assert(0 || 12);
assert("A" && "B");

// From developer.mozilla.org/en-US/docs/JavaScript/Guide/Expressions_and_Operators:
assert((true && true) === true);
assert((true && false) === false);
assert((false && true) === false);
assert((false && (3 == 4)) === false);
assert(("Cat" && "Dog") === "Dog");
assert((false && "Cat") === false);
assert(("Cat" && false) === false);
assert((true || true) === true);
assert((false || true) === true);
assert((true || false) === true);
assert((false || (3 == 4)) === false);
assert(("Cat" || "Dog") === "Cat");
assert((false || "Cat") === "Cat");
assert(("Cat" || false) === "Cat");

// Short-circuit evaluation (second operand should never be evaluated)
true || assert(false);
false && assert(false);

// Comparison

assert(1 < 2);
assert(1 < 1.0001);
assert(1 <= 2);
assert(2 <= 2);
assert(2 > 1);
assert(1.00000001 > 1);
assert(2 >= 2);
assert("a" < "b" && "b" > "a");
assert("aardvark" < "aardwolf" && "aardwolf" > "aardvark");
var x = 1;
assert(x < 2);
