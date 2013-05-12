// test_try.js
// -----------

(this.load || require)((this.load ? 'test' : '.') + '/tools/assertions.js');

var assertTriedAndCaught = function(f) {
  this.caught = false;
  this.tried = false;
  f();
  console.assert(this.tried && this.caught);
};

var nestedThrow  = function() {
  (function() {
    (function() {
      (function() {
        throw new Error();
      })();
    })();
  })();
};


test('try statement', function() {

  test('with console.assert (natively thrown exception)', function() {
    assertTriedAndCaught(function() {
      try {
        this.tried = true;
        console.assert(false);
      } catch(e) {
        console.assert(this.tried);
        this.caught = true;
      }
    });
  });

  test('with throw statement', function() {
    assertTriedAndCaught(function() {
      try {
        this.tried = true;
        throw new Error();
      } catch(e) {
        console.assert(this.tried);
        this.caught = true;
      }
    });
  });

  test('exception identifier bound with error object', function() {
    assertTriedAndCaught(function() {
      try {
        this.tried = true;
        throw new EvalError('the message');
      } catch (e) {
        this.caught = true;
        console.assert(e.message === 'the message');
        console.assert(e.name === 'EvalError');
        console.assert(e instanceof EvalError);
      }
    });
  });

  test('custom error messages', function() {
    assertTriedAndCaught(function() {
      try {
        this.tried = true;
        throw new EvalError('another message');
      } catch (someSpecialName) {
        this.caught = true;
        console.assert(someSpecialName.message === 'another message');
      }
    });
  });

  test('nested try statements', function() {
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
  });

  test('error thrown from call', function() {
    assertTriedAndCaught(function() {
      try {
        this.tried = true;
        nestedThrow();
      } catch (e) {
        this.caught = true;
      }
    });
  });

  test('finally runs last', function() {
    var final_ran = false;

    assertTriedAndCaught(function() {
      try {
        this.tried = true;
        console.assert(false);
      } catch(e) {
        this.caught = true;
      } finally {
        console.assert(this.tried && this.caught);
        final_ran = true;
      }
    });

    console.assert(final_ran);
  });

  test('finally always runs', function() {
    final_ran = false;

    try {
    } catch(e) {
    } finally {
      final_ran = true;
    }

    console.assert(final_ran);
  });
});
