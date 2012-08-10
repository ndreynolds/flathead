// test_while.js
// -------------

x = 0;
while(x < 5) {
  console.assert(x != 5);
  x = x + 1;
}

console.assert(x == 5);
