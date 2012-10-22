#!/usr/bin/env node

// crunner.js
// ---------
// Finds, compiles and executes C tests.
//
// ./crunner.js [files]

var exec = require('child_process').exec,
    fs = require('fs'),
    passed = 0,
    failed = 0,
    found = 0,
    showOutput = false;

var colors = {
  success: '\033[34m',
  failure: '\033[31m',
  reset: '\033[0m'
};

var allDone = function() {
  console.log('\n ' + passed + ' passed,', failed + ' failed');
};

var onTestPass = function(fileName, stdout) {
  passed++;
  console.log(colors.success + '✓ ' + fileName + colors.reset);
  if (showOutput) console.log(stdout);
  if (passed + failed === found) allDone();
};

var onTestFail = function(fileName, stdout, stderr) {
  failed++;
  console.log(colors.failure + '✖ ' + fileName + colors.reset);
  console.log('\n' + stderr + '\n');
  if (showOutput) console.log(stdout);
  if (passed + failed === found) allDone();
};

var runScript = function(fileName, onSuccess, onFailure) {
  var baseName = fileName.split('/')[fileName.split('/').length - 1],
      outFile = fileName + '.out',
      compile = ['gcc', fileName, '-o', outFile, '-lm'].join(' '),
      execCmd = baseName === fileName ? './' + outFile : outFile;
  exec(compile, function(err, stdout, stderr) {
    if (err || stderr) {
      console.error(
        [colors.failure, "Error while attempting to compile ",
         baseName, colors.reset, "\n\n"].join(''),
        stderr
      );
      process.exit(1);
    }
    exec(execCmd, function(err, stdout, stderr) {
      return (err || stderr) ?
        onFailure(baseName, stdout, stderr) :
        onSuccess(baseName, stdout);
    });
  });
};

var runAll = function() {
  fs.readdir(__dirname, function(err, files) {
    files.forEach(function(f) {
      if (f.match(/^test_/)) {
        runScript([__dirname, f].join('/'), onTestPass, onTestFail);
        found++;
      }
    });
    console.log('Found ' + found + ' test files.\n  ');
  });
};

var runEach = function(files) {
  found = files.length;
  files.forEach(function(f) {
    runScript(f, onTestPass, onTestFail);
  });
};

var afterSetup = function(cb) {
  var pattern = __dirname + '/test_*.c.out';
  exec('rm ' + pattern, function(err) {
    cb();
  });
};

afterSetup(function() {
  if (process.argv.length > 2) {
    showOutput = true;
    runEach(process.argv.slice(2));
  } else {
    runAll();
  }
});
