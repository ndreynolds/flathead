// runner.js
// =========
// Runs the JavaScript test suite on flathead and other implementations.

var commander = require('commander'),
    path      = require('path'),
    exec      = require('child_process').exec,
    fs        = require('fs'),
    _         = require('underscore');


// Formatting
// ----------

// Python style string formatting
var format = function(str) {
  var args = _.toArray(arguments).slice(1);
  while(args.length) {
    if (!/%s/.test(str)) return str;
    str = str.replace(/%s/, args.shift());
  }
  return str;
};

// ANSI color wrappers
var colors = {
  success: function(str) {
    return '\033[34m' + str + '\033[0m';
  },
  failure: function(str) {
    return '\033[31m' + str + '\033[0m';
  }
};


// TestRunner
// ----------

var TestRunner = module.exports = function(options) {
  options = options || {};
  _.extend(this.options, options);
  if (this.options.dir)
    this.options.dir = fs.realpathSync(this.options.dir);
};

// Command line entry point
TestRunner.run = function() {
  commander.version('0.0.1')
    .usage('[options] [test.js]')
    .option('-d, --dir <path>',        'set the test directory', String)
    .option('-x, --exec <path>',       'executable to run the tests with', String)
    .option('-a, --args-tpl [string]', 'template for arguments to pass to exec', String)
    .option('-t, --timeout [ms]',      'kill test execution after', Number)
    .option('-q, --quiet',             'only display failed tests', Boolean)
    .parse(process.argv)
    .name = 'test';

  var runner = new TestRunner(commander);
  runner.start();
  if (commander.args.length)
    return runner.runEach(commander.args);
  return runner.runAll();
};

// Define the TestRunner prototype
_.extend(TestRunner.prototype, {

  options: {
    exec: null,
    dir: null,
    argsTpl: '[test]',
    timeout: 2000,
    quiet: false,
    files: []
  },

  stats: {
    passed: 0,
    failed: 0,
    found: 0,
    startedAt: 0,
    done: function() {
      return this.passed + this.failed === this.found;
    }
  },

  // Print to stdout unless quiet
  print: function(msg, force) {
    if (!this.options.quiet || force) console.log(msg);
  },

  error: function(msg) {
    console.error(msg);
  },

  // Print a success message and update the stats.
  onTestPass: function(fileName) {
    this.stats.passed++;
    if (!this.options.quiet)
      this.print(colors.success(format('✓ %s', fileName)));
    if (this.stats.done()) this.finish();
  },

  // Print a failure message and update the stats.
  onTestFail: function(fileName, err, stderr) {
    this.stats.failed++;
    this.error(colors.failure(format('✖ %s', fileName)));
    this.error(err);
    this.error(stderr);
    if (this.stats.done()) this.finish();
  },

  // Print the test summary.
  finish: function() {
    var duration = Date.now() - this.stats.startedAt;
    var color = colors[this.stats.failed > 0 ? 'failure' : 'success'];
    var msg = color(format('\n%s passed, %s failed', this.stats.passed, this.stats.failed));
    msg += format(' (%s) %sms\n', this.options.exec, duration);
    this.print(msg, true);
    if (this.stats.failed > 0)
      process.exit(1);
  },

  // Start collecting stats.
  start: function() {
    this.stats.startedAt = Date.now();
  },

  // Execute the given (or default) command providing the file as an argument.
  // Provide success/failure callbacks. A test is considered failed if the
  // exit code is non-zero, or the output streams are non-empty.
  runScript: function(fileName, onSuccess, onFailure) {
    var args = this.options.argsTpl.replace('[test]', fileName);
    var cmd = [this.options.exec, args].join(' ');
    exec(cmd, {timeout: this.options.timeout}, function(err, stdout, stderr) {
      fileName = fileName.split('/')[fileName.split('/').length - 1];
      return (err || stderr) ?
        onFailure(fileName, err, stderr) :
        onSuccess(fileName);
    });
  },

  // Look for test files in this file's directory and run them.
  runAll: function() {
    var this_ = this;
    fs.readdir(this.options.dir, function(err, files) {
      files.forEach(function(f) {
        if (f.match(/^test_/)) {
          this_.runScript(
            path.join(this_.options.dir, f),
            _.bind(this_.onTestPass, this_),
            _.bind(this_.onTestFail, this_)
          );
          this_.stats.found++;
        }
      });
      this_.print(format('Found %s test files.\n', this_.stats.found));
    });
  },

  // Run each file in an array of filename strings.
  runEach: function(files) {
    this.stats.found = files.length;
    files.forEach(function(f) {
      this.runScript(f, _.bind(this.onTestPass, this), _.bind(this.onTestFail, this));
    }, this);
  },
});
