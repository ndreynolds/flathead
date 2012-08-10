#!/usr/local/bin/node

// runner.js
// ---------
// Finds and runs the JavaScript test suite on jslite, breaking on non-empty stdout. 
// Run this with NodeJS.

var exec = require('child_process').exec,
    fs = require('fs'),
    cmd = __dirname + '/../bin/jslite',
    passed = 0,
    failed = 0,
    found = 0;

var onTestPass = function(fileName) {
  passed++;
  console.log("✓ " + fileName);
  if (passed + failed == found) allDone();
};

var onTestFail = function(fileName, stderr) {
  failed++;
  console.log("✖ " + fileName);
  console.log("\n" + stderr + "\n");
  if (passed + failed == found) allDone();
};

var allDone = function() {
  console.log("\n" + passed + " passed,", failed + " failed")
};

var runAll = function() {
  fs.readdir(__dirname, function(err, files) {
    files.forEach(function(f) {
      if (f.match(/^test_/)) {
        runScript(f, onTestPass, onTestFail);
        found++;
      }
    });
    console.log("Found " + found + " test files.\n  ");
  });
};

var runScript = function(fileName, onSuccess, onFailure) {
  var child = exec([cmd, fileName].join(' '), function(err, stdout, stderr) {
    if (!err && !stdout && !stderr) return onSuccess(fileName);
    onFailure(fileName, stderr);
  });
};

runAll();
