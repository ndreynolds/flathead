// test_switch.js
// --------------
// Switch Statement

var x = '42';

switch(x) {
  case 12:
    console.assert(false);
    break;
  case '42':
  case 'hello':
    break;
  default:
    console.assert(false);
    break;
}

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
    break;
}
