var assert = require("assert"),
    fixture = require("./utils").fixture;
    withParseTree = require("./utils").withParseTree;

describe('Expressions', function() {

  describe('Arithmetic', function() {

    it('should handle 2+2', function(done) {
      withParseTree('2+2;', function(tree) {
        assert.equal(tree, fixture('expression1'));
        done();
      });
    });

    it('should handle 6%(12-2*7)*3+9/1', function(done) {
      withParseTree('6%(12-2*7)*3+9/1;', function(tree) {
        assert.equal(tree, fixture('expression2'));
        done();
      });
    });

    it('should give * precedence over /', function(done) {
      withParseTree('1*2/3;', function(tree) {
        assert.equal(tree, fixture('expression3'));
        done();
      });
    });

    it('should give + precedence over -', function(done) {
      withParseTree('1+2-3;', function(tree) {
        assert.equal(tree, fixture('expression4'));
        done();
      });
    });
  });

  describe('12.4 Ambiguities', function() {

    it('should properly distinguish object literals from blocks', function(done) {
      // block and unary + operator on an object literal
      withParseTree('{}+{};', function(tree) {
        assert.equal(tree, fixture('block-object1'));
        done();
      });
    });

    it('should properly distinguish object literals from blocks (2)', function(done) {
      // block and unary + operator on an array literal
      withParseTree('{}+[];', function(tree) {
        assert.equal(tree, fixture('block-object2'));
        done();
      });
    });

    it('should properly distinguish object literals from blocks (3)', function(done) {
      // binary + operator on array literal and object literal
      withParseTree('[]+{};', function(tree) {
        assert.equal(tree, fixture('block-object3'));
        done();
      });
    });
  });

});
