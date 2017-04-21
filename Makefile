# Makefile
#
# Copyright (c) 2012-2013 Nick Reynolds
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
CFLAGS = -Wall -Wextra -Wno-unused-parameter -O3 -std=c99 -pedantic -D_XOPEN_SOURCE

HAS_FPU = yes
MFPU =

CPUINFO = /proc/cpuinfo
HAS_CPUINFO = $(shell test -e $(CPUINFO) && echo yes)

YACC = bison
YACC_FLAGS = -y -d -t -v

LEX = flex
LEX_FLAGS =

LIBS = -I/usr/local/include -I/usr/include -L/usr/local/lib -L/usr/lib -lm
OBJ_FILES = y.tab.o lex.yy.o src/eval.o src/str.o src/regexp.o src/cli.o \
src/nodes.o src/args.o src/flathead.o src/debug.o src/gc.o src/props.o \
src/runtime/runtime.o src/runtime/lib/Math.o src/runtime/lib/RegExp.o \
src/runtime/lib/Error.o src/runtime/lib/String.o src/runtime/lib/console.o \
src/runtime/lib/gc.o src/runtime/lib/Function.o src/runtime/lib/Object.o \
src/runtime/lib/Boolean.o src/runtime/lib/Number.o src/runtime/lib/Date.o \
src/runtime/lib/Array.o

OUT_FILE = bin/flat
YACC_FILE = src/grammar.y
LEX_FILE = src/lexer.l

TEST_FLAGS = --dir test

VERSION = $(shell grep -o '\d.\d.\d' src/version.h)


# Configuration

ifeq ($(HAS_CPUINFO), yes)
  HAS_FPU = $(shell cat $(CPUINFO) | grep fpu | grep yes > /dev/null && echo yes)
  MFPU = $(shell cat $(CPUINFO) | grep -i vfp | tr ' ' '\n' | grep -i vfp | tail -1)
endif


# Build flags, usage: make debug=on

ifeq ($(debug), on)
  CFLAGS += -DFH_DEBUG
endif

ifdef hashfn
  CFLAGS += -DHASH_FUNCTION=$(hashfn)
endif

ifeq ($(gcprofile), on)
  CFLAGS += -DFH_GC_PROFILE
endif

ifneq ($(gcexpose), off)
  CFLAGS += -DFH_GC_EXPOSE
endif

ifeq ($(readline), off)
  CFLAGS += -DFH_NO_REPL
else
  LIBS += -lreadline
endif

ifeq ($(regexp), off)
  CFLAGS += -DFH_NO_REGEXP
else
  LIBS += -lpcre
endif

ifneq ($(HAS_FPU), yes)
  CFLAGS += -mfloat-abi=softfp -mfpu=$(MFPU)
endif

.PHONY: test ctest

all: default

debug: CFLAGS += -g -O0
debug: clean default

malloc-debug: CC = gcc-4.9
malloc-debug: LIBS += -lduma
malloc-debug: debug


lex.yy.c: $(LEX_FILE)
	$(LEX) --header-file=lex.yy.h $(LEX_FLAGS) $(LEX_FILE)

lex.yy.o: lex.yy.c $(LEX_FILE)
	@echo "[CC -o] lex.yy.o"
	@$(CC) -c $(CFLAGS) lex.yy.c -o lex.yy.o

y.tab.o: lex.yy.c $(YACC_FILE)
	$(YACC) $(YACC_FLAGS) $(YACC_FILE)
	@echo "[CC -o] y.tab.o"
	@$(CC) -c $(CFLAGS) y.tab.c -o y.tab.o

linker: $(OBJ_FILES)

.c.o:
	@echo "[CC -c] $@"
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf y.* lex.yy.? y.tab.? $(OUT_FILE) $(OBJ_FILES)

install:
	cp $(OUT_FILE) /usr/local/bin/

default: linker
	@echo "[CC -o] $(OUT_FILE)"
	@$(CC) -o $(OUT_FILE) $(OBJ_FILES) $(LIBS)


ctest:
	node ctest/crunner.js

test: 
	bin/test $(TEST_FLAGS) -x bin/flat

test-node:
	bin/test $(TEST_FLAGS) -x node

test-v8:
	bin/test $(TEST_FLAGS) -x v8 -a "test/tools/harness.js [test]"

test-sm:
	bin/test $(TEST_FLAGS) -x js -a "\-f test/tools/harness.js \-f [test]"

test-rhino:
	bin/test $(TEST_FLAGS) -x rhino -t 10000 -a "\-f test/tools/harness.js \-f [test]"

test-all: TEST_FLAGS += --quiet
test-all: test test-node test-v8 test-sm test-rhino

test-grammar:
	node_modules/mocha/bin/mocha test/grammar


archive:
	git archive --format=tar.gz --prefix="flathead-$(VERSION)/" \
		--output="flathead-$(VERSION).tar.gz" master .
