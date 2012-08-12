#!/usr/bin/env node

// runner.js
// ---------
// Finds and runs the JavaScript test suite on jslite. 
//
// Run this script with NodeJS.
//
// --command     Specify an alternate executable to run the test files with. 
//               (e.g. 'node' or 'js')   

var exec = require('child_process').exec,
    fs = require('fs'),
    _ = require('./underscore.js');

var testRunner = exports.testRunner = {

  options: {
    cmd: __dirname + '/../bin/jslite',
    files: []
  },

  stats: {
    passed: 0,
    failed: 0,
    found: 0,
    done: function() {
      return this.passed + this.failed == this.found;
    }
  },

  colors: {
    success: '\033[34m',
    failure: '\033[31m',
    reset: '\033[0m'
  },

  parseArgs: function() {
    var args = process.argv;
    if ((index = args.indexOf('--command')) >= 0) {
      this.options.cmd = args[index + 1];
      args = _.without(args, args[index], args[index + 1]);
    }
    if (args.length > 2)
      this.options.files = args.slice(2);
  },

  onTestPass: function(fileName) {
    this.stats.passed++;
    console.log(this.colors.success + '✓ ' + fileName + this.colors.reset);
    if (this.stats.done()) this.finish();
  },

  onTestFail: function(fileName, stderr) {
    this.stats.failed++;
    console.log(this.colors.failure + '✖ ' + fileName + this.colors.reset);
    console.log('\n' + stderr + '\n');
    if (this.stats.done()) this.finish();
  },

  finish: function() {
    console.log(
      '\n', this.stats.passed + ' passed,', this.stats.failed + ' failed'
    );
  },

  runScript: function(fileName, onSuccess, onFailure) {
    var cmd = [this.options.cmd, fileName].join(' ');
    exec(cmd, function(err, stdout, stderr) {
      fileName = fileName.split('/')[fileName.split('/').length - 1]
      return (err || stdout || stderr) ? 
        onFailure(fileName, stderr) : 
        onSuccess(fileName);
    });
  },

  runAll: function() {
    var that = this;
    fs.readdir(__dirname, function(err, files) {
      files.forEach(function(f) {
        if (f.match(/^test_/)) {
          that.runScript(
            [__dirname, f].join('/'), 
            that.onTestPass, 
            that.onTestFail
          );
          that.stats.found++;
        }
      });
      console.log('Found ' + that.stats.found + ' test files.\n  ');
    });
  },

  runEach: function(files) {
    this.stats.found = files.length;
    files.forEach(function(f) {
      this.runScript(f, this.onTestPass, this.onTestFail);
    }, this);
  },

  run: function() {
    this.parseArgs();
    if (this.options.files.length)
      return this.runEach(this.options.files);
    return this.runAll();
  }
};

_.bindAll(testRunner);

if (require.main === module) testRunner.run();
