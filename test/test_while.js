// test_while.js
// -------------

var assert = console.assert;


// Basic while-loop

var x = 0;
while(x < 5) {
  console.assert(x < 5);
  x++;
}
console.assert(x === 5);


// Using control statements

while(true) {
  break;
}

var i = 0;
var j = 0;
while(i < 10) {
  i++;
  continue;
  j++;
}

assert(i === 10);
assert(j === 0);
