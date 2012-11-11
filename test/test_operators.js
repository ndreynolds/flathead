// test_operators.js
// -----------------

var assert = console.assert;

var assertEquals = function(a, b) {
  if (a !== b)
    console.log(a + ' !== ' + b);
  assert(a === b);
};

var test = function(name, f) {
  f();
};


// ------------------------------------------------------------------
// UNARY
// ------------------------------------------------------------------

test('unary prefix operators', function() {

  test('increment', function() {
    var numString = "3.14";
    var a = 0;
    assertEquals(1, ++a);
    var alphaString = "abc";
    assert(isNaN(++alphaString));
    var result = ++numString;
    // Handle IEEE precision quirks
    assert(result >= 4.14 && result <= 4.14 + 0.0000001);
  });

  test('decrement', function() {
    var b = 2;
    assertEquals(1, --b);
  });

  test('unary +', function() {
    var numString = "3.14";
    assertEquals(3.14, +numString);
    assert(isNaN(+{}));
  });

  test('unary -', function() {
    var numString = "3.14";
    assertEquals(-3.14, -numString);
    assert(-Infinity !== Infinity);
  });

  test('negation', function() {
    var numString = "3.14";
    assert(!numString === false);
    assert(!!numString === true);
    assert(!false === true);
    assert(!undefined === true);
    assert(!!undefined === false);
  });

  test('typeof', function() {
    assert(typeof "hello" === "string");
    assert(typeof 42 === "number");
    assert(typeof true === "boolean");
    assert(typeof null === "object");
    assert(typeof undefined === "undefined");
    assert(typeof {a: 12} === "object");
    assert(typeof [1, 2, 3] === "object");
    assert(typeof function() { return 42; } === "function");
    assert(typeof '1' + 12 === 'string12');
  });

  test('void', function() {
    assert(void 'boom' === undefined);
    assert(void 12 === undefined);

    var counter = 0;

    assert(void ++counter === undefined);
    assertEquals(counter, 1);
    assert(void (counter *= 4) === undefined);
    assertEquals(counter, 4);
  });

  test('delete', function() {
    // Also see test_delete.js
    var obj = {a: 42};
    delete obj.a;
    assertEquals(undefined, obj.a);
  });

});

test('unary postfix', function() {

  test('increment', function() {
    var c = 0;
    assert(c++ === 0);
    assert(c === 1);
  });

  test('decrement', function() {
    var d = 2;
    assert(d-- === 2);
    assert(d === 1);
  });

});


// ------------------------------------------------------------------
// BINARY
// ------------------------------------------------------------------

test('binary', function() {

  test('equality', function() {
    assert(true == true);
    assert(null == undefined);
    assert(Infinity == Infinity);
    assert(2 == '2');
    assert(0 == '');
    assert(0 == '0');
    assert(false == '0');
    var obj = {};
    assert(obj == obj);
  });

  test('strict equality', function() {
    assert(Infinity === Infinity);
    var obj = {};
    assert(obj === obj);
  });

  test('inequality', function() {
    assert(true != false);
    assert(NaN != NaN);
    assert([] != null);
    assert({} != null);
    assert('' != '0');
    assert(false != 'false');
    assert(false != undefined);
    assert(false != null);
    assert({} != {});
  });

  test('strict inequality', function() {
    assert(null !== undefined);
    assert(NaN !== NaN);
    assert(2 !== '2');
    assert({} !== {});
  });

  test('plus (addition)', function() {
    assert((2 + 2) === 4);
  });

  test('plus (concatenation)', function() {
    assert(("Hello" + "Goodbye") === "HelloGoodbye");
    assert(("Hello" + 12) === "Hello12");
    assert(("Hello" + undefined) === "Helloundefined");
    assert(("Hello" + null) === "Hellonull");
    assert(("Hello" + true) === "Hellotrue");
    assert(("Hello" + false) === "Hellofalse");
  });

  test('subtraction', function() {
    assert((3 - 7) === -4);
  });

  test('multiplication', function() {
    assert((10 * 16) === 160);
    assert(isNaN(NaN * 42));
    assert(isNaN(Infinity * 0));
    assertEquals(Infinity, Infinity * Infinity);
    assertEquals(-Infinity, Infinity * -Infinity);
    assertEquals(Infinity, -Infinity * -Infinity);
    assertEquals(Infinity, Infinity * 42);
    assertEquals(-Infinity, Infinity * -9);
    assertEquals(Infinity, -Infinity * -1);
    assertEquals(Infinity, 1e200 * 1e200);
  });

  test('division', function() {
    assert((9 / 3) === 3);
  });

  test('modulus', function() {
    assert((10 % 7) === 3);
  });

  test('logical or', function() {
    assert(0 || 12);
    assert((true || true) === true);
    assert((false || true) === true);
    assert((true || false) === true);
    assert((false || (3 == 4)) === false);
    assert(("Cat" || "Dog") === "Cat");
    assert((false || "Cat") === "Cat");
    assert(("Cat" || false) === "Cat");

    // Must short circuit
    true || assert(false);
  });

  test('logical and', function() {
    assert("A" && "B");
    assert((true && true) === true);
    assert((true && false) === false);
    assert((false && true) === false);
    assert((false && (3 == 4)) === false);
    assert(("Cat" && "Dog") === "Dog");
    assert((false && "Cat") === false);
    assert(("Cat" && false) === false);

    // Must short circuit
    false && assert(false);
  });

  test('less-than', function() {
    assert(1 < 2);
    assert(1 < 1.0001);
    var x = 1;
    assert(x < 2);
  });

  test('less-than-or-equal', function() {
    assert(1 <= 2);
    assert(2 <= 2);
  });

  test('greater-than', function() {
    assert(2 > 1);
    assert(1.00000001 > 1);
  });

  test('greater-than-or-equal', function() {
    assert(2 >= 2);
  });

  test('mixed relations', function() {
    assert("a" < "b" && "b" > "a");
    assert("aardvark" < "aardwolf" && "aardwolf" > "aardvark");
  });

  test('instanceof', function() {
    // TODO
  });

});
