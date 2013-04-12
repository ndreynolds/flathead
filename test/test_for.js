// test_for.js
// -----------

var assert = console.assert;


// -----------------------------------------------------------------------------
// Ensure all 8 forms are working
// -----------------------------------------------------------------------------

var i = 0;


// for ( exp ; exp ; exp ) stmt

for (i = 10; i > 0; i--) {
  assert(i > 0);
}
assert(i === 0);


// for ( exp ; exp ; ) stmt

for (i = 4; i < 10;) {
  assert(i >= 4);
  assert(i < 10);
  i++;
}
assert(i === 10);


// for ( exp ; ; exp ) stmt

for (i = 50 ;; i++) {
  assert(i >= 50);
  if (i === 100) break;
}
assert(i === 100);


// for ( exp ; ; ) stmt

for (i = 0;;) {
  if (i > 10) break;
  i++;
}
assert(i === 11);


// for ( ; exp ; exp ) stmt

for (; i > -50; i--) {
  assert(i <= 11);
  assert(i > -50);
}
assert(i === -50);


// for ( ; exp ; ) stmt

for (; i < 12 ;) {
  assert(i >= -50);
  i++;
}
assert(i === 12);


// for ( ; ; exp ) stmt

for (;; i++) {
  assert(i >= 12);
  if (i === 200) break;
}
assert(i === 200);


// for ( ; ; ) stmt

var stop = false;
for (;;) {
  if (stop) break;
  stop = true;
}
assert(stop);
