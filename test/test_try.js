// test_try.js
// -----------

// A few basic tests

try {
  console.assert(false);
} catch(e) {
}

try {
  throw new Error();
} catch(e) {
}

try {
  throw new EvalError('the message');
} catch (e) {
  console.assert(e.message === 'the message');
  console.assert(e.name === 'EvalError');
  console.assert(e instanceof EvalError);
}

try {
  throw new EvalError('another message');
} catch (someSpecialName) {
  console.assert(someSpecialName.message === 'another message');
}

try {
  try {
    throw new Error('from inner try');
  } catch (e) {
    console.assert(e.message === 'from inner try');
    throw new Error('from inner catch');
  }
} catch (e) {
  console.assert(e.message === 'from inner catch');
}
