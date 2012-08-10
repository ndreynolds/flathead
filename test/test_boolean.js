// test_boolean.js
// ---------------
// Tests adapted from v8's msjunit

console.assert(true == !false);
console.assert(false == !true);
console.assert(true == !!true);
console.assert(false == !!false);
