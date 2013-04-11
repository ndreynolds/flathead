// harness.js
// ----------
// Normalize testing between implementations.


// Define a basic console for implementations that do not provide one.

if (typeof console !== 'object') {
  console = {
    log: function() {
      var args = Array.prototype.slice.call(arguments);
      for (var i = 0; i < args.length; i++)
        print(args[i]);
    },

    error: function() {
      var args = Array.prototype.slice.call(arguments);
      for (var i = 0; i < args.length; i++)
        print(args[i]);
    },

    assert: function(assertion) {
      if (!assertion)
        throw new Error("assertion failed");
    }
  };
}

// If this isn't available, the assertions.js script should be included as a
// command-line argument. Seems to be present in at least V8, SpiderMonkey and
// Rhino.

if (typeof load !== 'function') {
  load = function(scriptName) {
    if (!assertionsLoaded)
      throw new Error("Missing assertion helpers. Include tools/assertions.js");
  };
}
