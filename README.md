Flathead
========
Flathead is a tiny JavaScript (Ecma-262) interpreter written in C.

![Flathead's REPL](http://ndreynolds.com/img/flathead.png)

The interpreter does a direct evaluation of the parse tree - it does not
currently build any further IR or perform any optimizations. As a result, it
starts up very quickly, and performs well on code that wouldn't benefit much
from optimization, and less well on code that would (e.g. loops). 

One of the goals has been to keep Flathead portable. A minimal build won't
require installing any additional dependencies on most systems. It supports
x86, x86_64 and ARM.

Flathead comes with the full EcmaScript runtime (i.e. the Date, Math, Array,
and other global objects) as well as a console object.

It's nearly complete, but is still missing a few features - see [the Docket](#the-docket).


Installing
----------
Download or clone the source and run `make` within. This creates an executable
at `bin/flat` within the source directory. (You can optionally run `make
install` to copy this to `/usr/local/bin/`.)

Flathead should build on most POSIX platforms (no Windows support yet).

#### Dependencies 

If you received any errors, you may just be missing dependencies.

`flex` and `bison` are required to generate the lexer and parser. (Most systems
will already have these.)

The default build depends on GNU Readline `lreadline` for the REPL, and PCRE
`lpcre` or the regular expression implementation. If you don't have these, or
don't need the features, you can run `make readline=off regexp=off` to compile
without them.

If you're still having trouble building, please create an issue.


Running
-------
Once built, you can run `bin/flat` without arguments to start a REPL:

    $ bin/flat
    > 2 + 2
    4
    >

Or with a script as argument:

    $ bin/flat say_hello.js
    Hello!

View the parse tree with `-n`:

    $ bin/flat -n
    > 2 + 2
    source list
      expression statement
        expression (binary)
          +
          number (2.000000)
          number (2.000000)
    4
    >

See all the options with `-h`:

    $ bin/flat -h
    Usage: flat [options] [script.js] 

    Options:
      -v, --version       print version info
      -h, --help          print this help text
      -i, --interactive   force REPL
      -n, --nodes         print the AST
      -t, --tokens        print tokens


Running the tests
-----------------
Flathead's test suite can be run against the `bin/flat` executable, as well as
other EcmaScript implementations (assuming you have them installed) – this is
often useful to check that the tests themselves are correct. Some of the tests
verify behavior that is implementation-specific. The goal has been to match
V8's behavior in these cases.

The test runner itself (`runner.js`) requires Node.js. It locates files with
names beginning in `test_` and executes them with the configured implementation
and options. 

The Makefile has a few shortcuts:

`make test` to run with Flathead's `bin/flat` executable.  
`make test-v8` to run using `v8`.   
`make test-node` to run using `node`.  
`make test-sm` to run using `js` (SpiderMonkey).  
`make test-rhino` to run using `rhino`.  


The Docket
----------
- Try statements (`try`, `catch`, `finally`)
- Labels (e.g. `loop1: ...; continue loop1;`)
- Automatic Semicolon Insertion
- Unicode (might work, but currently unsupported)
- The JSON object ([JSON-js][1] can be used as polyfill)
- RegExp#exec
- String#match
- String#replace
- `new Function(...)`


[1]: http://github.com/douglascrockford/JSON-js
