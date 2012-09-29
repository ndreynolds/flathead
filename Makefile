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
CFLAGS = -Wall 
YACC = bison -y -d -t -v
LEX = flex

SRC_FILES = $(wildcard src/*.c)
LIB_FILES = $(wildcard lib/*.c)

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

ifeq ($(gcexpose), on)
  CFLAGS += -Dfh_gc_expose
endif

ifneq ($(readline), off)
  CFLAGS += -lreadline
endif

.PHONY: test

all: clean default

test: 
	$(MAKE) gcexpose=on
	node test/runner.js
	node ctest/crunner.js

debug: CFLAGS += -g -O0
debug: default

lexer:
	$(LEX) $(LEX_FILE)

parser: 
	$(YACC) $(YACC_FILE)

clean:
	rm -rf y.* lex.yy.c bin/fh* a.out

install: default
	cp bin/fh /usr/local/bin/
	
default: parser lexer 
	$(CC) $(CFLAGS) -o $(OUT_FILE) $(YACC_OUT) $(LEX_OUT) $(SRC_FILES) $(LIB_FILES) -lm
