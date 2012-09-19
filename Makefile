# Makefile
# --------
# flathead 

CC=gcc
CFLAGS=-Wall
YACC=bison -y -d -t -v
LEX=flex

MAIN=src/flathead.c
SRC_FILES=src/nodes.c src/gc.c src/eval.c src/runtime.c
LIB_FILES=lib/console.c lib/Math.c lib/Number.c lib/Object.c

OUT_FILE=-o bin/fh
GRAMMAR_FILE=src/grammar.y
LEX_FILE=src/lexer.l
LEX_OUT=lex.yy.c
YACC_OUT=y.tab.c

all: clean default

debug: CFLAGS += -g -O0 -fno-inline
debug: default

tests:
	node test/runner.js
	node ctest/crunner.js

grammar:
	$(YACC) $(GRAMMAR_FILE)

lexer:
	$(LEX) $(LEX_FILE)

clean:
	rm -rf y.* lex.yy.c bin/fh* a.out

install: default
	cp bin/fh /usr/local/bin/
	
default: grammar lexer
	$(CC) $(CFLAGS) $(OUT_FILE) $(YACC_OUT) $(LEX_OUT) $(MAIN) $(LIB_FILES) $(SRC_FILES) -lm
