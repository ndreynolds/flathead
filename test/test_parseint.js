// test_parseint.js
// ----------------

(this.load || require)((this.load ? 'test' : '.') + '/tools/assertions.js');


test('without radix', function() {

  assertEquals(42, parseInt("42"));
  assertEquals(3,  parseInt("3.14"));
  assertNaN(parseInt("Not a number"));
  assertNaN(parseInt(Infinity));

  assertEquals(0, parseInt('0'));
  assertEquals(0, parseInt(' 0'));
  assertEquals(0, parseInt(' 0 '));

  testIfFlathead(function() {
    // Depending on who you ask, these may be 63 or 77 due to the breaking
    // changes in how octals were handled between the 3rd and 5th editions.
    // (See Annex E)
    assertEquals(77,  parseInt('077'));
    assertEquals(77,  parseInt('  077'));
    assertEquals(77,  parseInt('  077   '));
    assertEquals(-77, parseInt('  -077'));
  });

  assertEquals(0x16, parseInt('0x16'));
  assertEquals(0x16, parseInt('0X16'));
  assertNaN(parseInt('0XX16'));

  assertClose(3.2512351e+27, parseInt('3251235123512351235123512351'), 0.00001e+27);
});

test('with radix', function() {
  // Examples from MDN's parseInt reference, msjunit

  assertEquals(15, parseInt(" 0xF", 16));
  assertEquals(15, parseInt(" F", 16));
  assertEquals(15, parseInt("17", 8));

  // Note that in the following line, the octal of "17" is taken.
  assertEquals(15, parseInt(021, 8));
  assertEquals(17, parseInt('021', 8));

  assertEquals(15, parseInt("015", 10));
  assertEquals(15, parseInt(15.99, 10));
  assertEquals(15, parseInt("FXX123", 16));
  assertEquals(15, parseInt("1111", 2));
  assertEquals(15, parseInt("15*3", 10));
  assertEquals(15, parseInt("15e2", 10));
  assertEquals(15, parseInt("15px", 10));
  assertEquals(15, parseInt("12", 13));

  assertEquals(0x12, parseInt('0x12'));
  assertEquals(0x12, parseInt('0x12', 16));
  assertEquals(0x12, parseInt('0x12', 16.1));
  assertEquals(0x12, parseInt('0x12', NaN));

  assertEquals(-15, parseInt("-F", 16));
  assertEquals(-15, parseInt("-0F", 16));
  assertEquals(-15, parseInt("-0XF", 16));
  assertEquals(-15, parseInt(-15.1, 10));
  assertEquals(-15, parseInt(" -17", 8));
  assertEquals(-15, parseInt(" -15", 10));
  assertEquals(-15, parseInt("-1111", 2));
  assertEquals(-15, parseInt("-15e1", 10));
  assertEquals(-15, parseInt("-12", 13));

  assertNaN(parseInt("Hello", 8));
  assertNaN(parseInt("546", 2));
  assertNaN(parseInt("Not a number", 10));

  assertEquals(0x16, parseInt('0X16', 16));
  assertEquals(0,    parseInt('0X16', 10));
});
