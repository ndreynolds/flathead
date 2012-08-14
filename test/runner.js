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

  // Parse command line arguments into the options object.
  parseArgs: function() {
    var args = process.argv;
    if ((index = args.indexOf('--command')) >= 0) {
      this.options.cmd = args[index + 1];
      args = _.without(args, args[index], args[index + 1]);
    }
    if (args.length > 2)
      this.options.files = args.slice(2);
  },

  // Print a success message and update the stats.
  onTestPass: function(fileName) {
    this.stats.passed++;
    console.log(this.colors.success + '✓ ' + fileName + this.colors.reset);
    if (this.stats.done()) this.finish();
  },

  // Print a failure message and update the stats.
  onTestFail: function(fileName, err, stderr) {
    this.stats.failed++;
    console.log(this.colors.failure + '✖ ' + fileName + this.colors.reset);
    console.error(err, '\n', stderr);
    if (this.stats.done()) this.finish();
  },

  // Print the test summary.
  finish: function() {
    console.log(
      '\n', this.stats.passed + ' passed,', this.stats.failed + ' failed'
    );
  },

  // Execute the given (or default) command providing the file as an argument.
  // Provide success/failure callbacks. A test is considered failed if the 
  // exit code is non-zero, or the output streams are non-empty.
  runScript: function(fileName, onSuccess, onFailure) {
    var cmd = [this.options.cmd, fileName].join(' ');
    exec(cmd, {timeout: 500}, function(err, stdout, stderr) {
      fileName = fileName.split('/')[fileName.split('/').length - 1]
      return (err || stdout || stderr) ? 
        onFailure(fileName, err, stderr) : 
        onSuccess(fileName);
    });
  },

  // Look for test files in this file's directory and run them.
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

  // Run each file in an array of filename strings.
  runEach: function(files) {
    this.stats.found = files.length;
    files.forEach(function(f) {
      this.runScript(f, this.onTestPass, this.onTestFail);
    }, this);
  },

  // Start running tests.
  run: function() {
    this.parseArgs();
    if (this.options.files.length)
      return this.runEach(this.options.files);
    return this.runAll();
  }
};

_.bindAll(testRunner);

if (require.main === module) testRunner.run();
