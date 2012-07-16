var assert = require("assert"),
    fixture = require("./utils").fixture;
    withParseTree = require("./utils").withParseTree;

describe('Expressions', function() {

  describe('Arithmetic', function() {

    it('should handle 2+2', function(done) {
      withParseTree("2+2;", function(tree) {
        assert.equal(tree, fixture("expression1"));
        done();
      });
    });

    it('should handle 6%(12-2*7)*3+9/1', function(done) {
      withParseTree("6%(12-2*7)*3+9/1;", function(tree) {
        assert.equal(tree, fixture("expression2"));
        done();
      });
    });

    it('should give * precedence over /', function(done) {
      withParseTree("1*2/3;", function(tree) {
        assert.equal(tree, fixture("expression3"));
        done();
      });
    })

    it('should give + precedence over -', function(done) {
      withParseTree("1+2-3;", function(tree) {
        assert.equal(tree, fixture("expression4"));
        done();
      });
    });
  });

});
