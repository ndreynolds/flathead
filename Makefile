# Makefile
# --------
# flathead 

COMPILER=gcc
YACC=bison -y -d -t -v
LEX=flex

MAIN=src/flathead.c
SRC_FILES=src/nodes.c src/eval.c src/runtime.c
LIB_FILES=lib/console.c lib/Math.c lib/Number.c
OUT_FILE=-o bin/fh
GRAMMAR_FILE=src/grammar.y
LEX_FILE=src/lexer.l
LEX_OUT=lex.yy.c
YACC_OUT=y.tab.c

all: clean default

tests:
	node test/runner.js
	node ctest/crunner.js

grammar:
	$(YACC) $(GRAMMAR_FILE)

lexer:
	$(LEX) $(LEX_FILE)

clean:
	rm -rf y.* lex.yy.c bin/fh* a.out

debug: grammar lexer
	$(COMPILER) -g $(OUT_FILE) $(YACC_OUT) $(LEX_OUT) $(MAIN) $(LIB_FILES) $(SRC_FILES) -lm
	gdb bin/fh

install: default
	cp bin/fh /usr/local/bin/
	
default: grammar lexer
	$(COMPILER) $(OUT_FILE) $(YACC_OUT) $(LEX_OUT) $(MAIN) $(LIB_FILES) $(SRC_FILES) -lm
