// test_instanceof.js
// ------------------

var assert = console.assert,
    assertTrue = assert,
    assertFalse = function(a) { assert(!a); };

var test = function(name, f) {
  f();
};


test('instanceof behavior of built-ins', function() {
  var fun = function() {},
      arr = [],
      obj = {},
      str = '',
      strObj = new String(''),
      num = 42,
      numObj = new Number(42),
      reg = /abc/,
      date = new Date();

  assertTrue(fun instanceof Function);
  assertTrue(function() {} instanceof Function);
  assertTrue(fun instanceof Object);

  assertTrue(arr instanceof Array);
  assertTrue([] instanceof Array);
  assertTrue(arr instanceof Object);
  assertFalse(arr instanceof Function);

  assertTrue(obj instanceof Object);
  assertTrue({} instanceof Object);

  assertFalse(str instanceof String);
  assertTrue(strObj instanceof String);

  assertFalse(num instanceof Number);
  assertTrue(numObj instanceof Number);

  assertTrue(date instanceof Date);
  assertTrue(date instanceof Object);

  assertTrue(reg instanceof RegExp);
  assertTrue(reg instanceof Object);

  assertTrue(Object instanceof Object);
  assertTrue(Object instanceof Function);
  assertTrue(Function instanceof Object);
  assertTrue(Function instanceof Function);

  assertTrue(String instanceof Object);
  assertTrue(Array instanceof Object);
  assertTrue(RegExp instanceof Object);
  assertTrue(Number instanceof Object);
  assertTrue(Boolean instanceof Object);
});


test('instanceof with user-defined constructors', function() {
  function Car(make, model) {}
  Car.prototype.turn = function() {};
  Car.prototype.brake = function() {};

  function RaceCar(make, model) {}
  RaceCar.prototype = new Car();
  RaceCar.prototype.nitrous = function() {};

  function Truck(make, model) {}
  Truck.prototype = new Car();
  Truck.prototype.tow = function() {};

  function GarbageTruck(make, model) {}
  GarbageTruck.prototype = new Truck();
  GarbageTruck.prototype.compact = function() {};

  var myCar = new Car('Ford', 'Focus');
  var myRaceCar = new RaceCar('Ferrari', 'Enzo');
  var myTruck = new Truck('Ford', 'F150');
  var myGarbageTruck = new GarbageTruck('Heil', 'Colectomatic');

  assertTrue(myCar instanceof Car);

  assertTrue(myRaceCar instanceof RaceCar);
  assertTrue(myRaceCar instanceof Car);

  assertTrue(myTruck instanceof Truck);
  assertTrue(myTruck instanceof Car);

  assertTrue(myGarbageTruck instanceof GarbageTruck);
  assertTrue(myGarbageTruck instanceof Truck);
  assertTrue(myGarbageTruck instanceof Car);
});
