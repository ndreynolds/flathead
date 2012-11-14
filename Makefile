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
CFLAGS = -Wall -O3
YACC = bison -y -d -t -v
LEX = flex

LIBS = -I/usr/local/include -I/usr/include -L/usr/local/lib -L/usr/lib -lm
SRC_FILES = $(wildcard src/*.c)
LIB_FILES = $(wildcard src/runtime/*.c) $(wildcard src/runtime/lib/*.c)

OUT_FILE = bin/flat
YACC_FILE = src/grammar.y
LEX_FILE = src/lexer.l
LEX_OUT = lex.yy.c
YACC_OUT = y.tab.c

# Build flags, usage: make debug=on

ifeq ($(debug), on)
  CFLAGS += -Dfh_debug
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

all: clean default

debug: CFLAGS += -g -O0
debug: default

malloc-debug: CC = gcc-4.7
malloc-debug: LIBS += -lefence
malloc-debug: debug

lexer:
	$(LEX) $(LEX_FILE)

parser: 
	$(YACC) $(YACC_FILE)

clean:
	rm -rf y.* lex.yy.c bin/fh* a.out

install:
	cp $(OUT_FILE) /usr/local/bin/

default: parser lexer 
	$(CC) $(CFLAGS) -o $(OUT_FILE) $(YACC_OUT) $(LEX_OUT) $(SRC_FILES) $(LIB_FILES) $(LIBS)

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
