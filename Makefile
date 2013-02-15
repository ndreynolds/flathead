# Makefile
#
# Copyright (c) 2012 Nick Reynolds
#  
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#  
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -O3 -std=c99 -pedantic -D_POSIX_SOURCE
YACC = bison -y -d -t -v
LEX = flex

LIBS = -I/usr/local/include -I/usr/include -L/usr/local/lib -L/usr/lib -lm
OBJ_FILES = y.tab.o lex.yy.o src/eval.o src/str.o src/regexp.o src/cli.o \
src/nodes.o src/flathead.o src/debug.o src/gc.o src/props.o \
src/runtime/runtime.o src/runtime/lib/Math.o src/runtime/lib/RegExp.o \
src/runtime/lib/Error.o src/runtime/lib/String.o src/runtime/lib/console.o \
src/runtime/lib/Function.o src/runtime/lib/Object.o src/runtime/lib/Boolean.o \
src/runtime/lib/Number.o src/runtime/lib/Date.o src/runtime/lib/Array.o

OUT_FILE = bin/flat
YACC_FILE = src/grammar.y
LEX_FILE = src/lexer.l

VERSION = 0.6

# Build flags, usage: make debug=on

ifeq ($(debug), on)
  CFLAGS += -Dfh_debug
endif

ifdef hashfn
  CFLAGS += -DHASH_FUNCTION=$(hashfn)
endif

ifeq ($(gcprofile), on)
  CFLAGS += -Dfh_gc_profile
endif

ifneq ($(gcexpose), off)
  CFLAGS += -Dfh_gc_expose
endif

ifeq ($(readline), off)
  CFLAGS += -Dfh_no_repl
else
  LIBS += -lreadline
endif

ifeq ($(regexp), off)
  CFLAGS += -Dfh_no_regexp
else
  LIBS += -lpcre
endif

.PHONY: test ctest

all: default

debug: CFLAGS += -g -O0
debug: default

malloc-debug: CC = gcc-4.7
malloc-debug: LIBS += -lefence
malloc-debug: debug

lex.yy.o: $(LEX_FILE)
	$(LEX) $(LEX_FILE)
	@echo "[CC -o] lex.yy.o"
	@$(CC) -c $(CFLAGS) lex.yy.c -o lex.yy.o

y.tab.o: $(YACC_FILE)
	$(YACC) $(YACC_FILE)
	@echo "[CC -o] y.tab.o"
	@$(CC) -c $(CFLAGS) y.tab.c -o y.tab.o

linker: $(OBJ_FILES)

.c.o:
	@echo "[CC -c] $@"
	@$(CC) -c $(CFLAGS) $< -o $@

make-bin:
	mkdir -p bin

clean:
	rm -rf y.* lex.yy.c bin/fh* a.out $(OBJ_FILES)

install:
	cp $(OUT_FILE) /usr/local/bin/

default: make-bin linker
	@echo "[CC -o] $(OUT_FILE)"
	@$(CC) -o $(OUT_FILE) $(OBJ_FILES) $(LIBS)

ctest:
	node ctest/crunner.js

test: 
	node test/runner.js

test-node:
	node test/runner.js --exec node

test-v8:
	node test/runner.js --exec v8 --args "test/harness.js [test]"

test-sm:
	node test/runner.js --exec js --args "-f test/harness.js -f [test]"

test-rhino:
	node test/runner.js --exec rhino --timeout 10000 --args "-f test/harness.js -f [test]"

test-grammar:
	mocha test/grammar

archive:
	git archive --format=tar.gz --prefix="flathead-$(VERSION)/" \
		--output="flathead-$(VERSION).tar.gz" master .
