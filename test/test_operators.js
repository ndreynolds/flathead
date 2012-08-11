// test_operators.js
// -----------------


// Prefix
a = 0;
console.assert(++a === 1);

b = 2;
console.assert(--b === 1);

numString = "3.14";

console.assert(+numString === 3.14);
console.assert(-numString === -3.14);
console.assert(!numString === false);
console.assert(!!numString === true);
console.assert(!false === true);

// IEEE precision 
result = ++numString;
console.assert(result >= 4.14 && result <= 4.14 + 0.0000001);

alphaString = "abc";
console.assert(isNaN(++alphaString));

// Postfix

c = 0;
console.assert(c++ === 0);
console.assert(c === 1);

d = 2;
console.assert(d-- === 2);
console.assert(d === 1);


// Binary
