// test_calls.js
// -------------

var myObject = {
  myMethod: function() {
    return 42;
  }
};

console.assert(myObject.myMethod() == 42);
console.assert(myObject["myMethod"]() == 42);
