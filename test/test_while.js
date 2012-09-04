// test_while.js
// -------------

var x = 0;
while(x < 5) {
  console.assert(x != 5);
  x++;
}

console.assert(x == 5);
