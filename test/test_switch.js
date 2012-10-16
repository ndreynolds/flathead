// test_switch.js
// --------------
// Switch Statement

var counter = 0;


// Test matching case

var x = 42;

switch(x) {
  case 12:
    console.assert(false);
    break;
  case 42:
    counter++;
    break;
  default:
    console.assert(false);
    break;
}

console.assert(counter === 1);


// Test default case

var y = 'not handled';

switch(y) {
  case 'a':
    console.assert(false);
    break;
  case 'b':
    console.assert(false);
    break;
  case 'c':
    console.assert(false);
    break;
  default:
    counter++;
    break;
}

console.assert(counter === 2);


// Test fall-through cases and missing breaks.

var z = 'cat';

switch(z) {
  case 'cat':
  case 'dog':
    counter++;
  case 'lizard':
    counter++;
}

console.assert(counter === 4);


// Test that cases that follow default are checked first.

switch(z) {
  case 'dog':
    console.assert(false);
    break;
  case 'mouse':
    console.assert(false);
    break;
  default:                   
    console.assert(false);    // This should NOT be executed
    break;
  case 'cat':
    counter++;
}

console.assert(counter === 5);
