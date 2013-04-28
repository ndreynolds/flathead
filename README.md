Flathead
========
Flathead is a tiny, portable JavaScript (ECMA-262) interpreter written in C.

![Flathead's REPL](http://ndreynolds.com/img/flathead.png)

The interpreter does a direct evaluation of the parse tree—it does not
currently build any further IR or perform any optimizations. As a result, it
starts up very quickly, and performs well on code that wouldn't benefit much
from optimization, and less well on code that would (e.g. loops). 

Flathead builds on Linux, OSX and \*BSD, on x86, x86_64 and ARM architectures.

Flathead comes with the full EcmaScript runtime (i.e. the Date, Math, Array
and other global objects) as well as a console object.

Most of the language is now implemented, you can see the remaining
work to be done on [the Docket](#the-docket).


Installing
----------
Download or clone the source and run `make` within. 

    git clone https://github.com/ndreynolds/flathead.git
    cd flathead
    make

This creates an executable at `bin/flat` within the source directory. (You can
optionally run `make install` to copy this to `/usr/local/bin/`.)

#### Dependencies 

If you received any errors, you may just be missing dependencies.

`flex` and `bison` are required to generate the lexer and parser.

The default build depends on GNU Readline (`lreadline`) for the REPL, and PCRE
(`lpcre`) for the regular expression implementation. They're not required,
however, for a minimal build. Run `make readline=off regexp=off` to compile
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
other EcmaScript implementations (assuming you have them installed).

The test runner itself requires Node.js. It locates files with names beginning
in `test_` and executes them with the configured implementation and options.
There are a few Node module dependencies, so you'll need to run `npm install`
before you can run the tests.

The Makefile has a few shortcuts:

`make test` to run with Flathead's `bin/flat` executable.  
`make test-v8` to run using `v8`.   
`make test-node` to run using `node`.  
`make test-sm` to run using `js` (SpiderMonkey).  
`make test-rhino` to run using `rhino`.  

There's also:

`make test-grammar` to verify parsing and AST formation 


The Docket
----------
- Try statements (`try`, `catch`, `finally`)
- With statement (`with`)
- Labels (e.g. `loop1: ...; continue loop1;`)
- Automatic Semicolon Insertion
- Unicode
- The JSON object ([JSON-js][1] can be used as polyfill)
- URI functions
- `String#replace`: replacement function
- `String#split`: RegExp separators


[1]: http://github.com/douglascrockford/JSON-js
