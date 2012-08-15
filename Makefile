# Makefile
# --------
# jslite 

COMPILER=gcc
YACC=bison -y -d -t -v
LEX=flex

SRC_FILES=src/nodes.c src/eval.c src/runtime.c  
LIB_FILES=lib/console.c lib/Math.c lib/Number.c
OUT_FILE=-o bin/jslite
GRAMMAR_FILE=src/grammar.y
LEX_FILE=src/lexer.l

all: clean default

tests:
	node test/runner.js
	node ctest/crunner.js

grammar:
	$(YACC) $(GRAMMAR_FILE)

lexer:
	$(LEX) $(LEX_FILE)

clean:
	rm -rf y.* lex.yy.c bin/jslite* a.out

debug: grammar lexer
	$(COMPILER) -g $(OUT_FILE) y.tab.c lex.yy.c src/jslite.c $(LIB_FILES) $(SRC_FILES)

install: default
	cp bin/jslite /usr/local/bin/
	
default: grammar lexer
	$(COMPILER) $(OUT_FILE) y.tab.c lex.yy.c src/jslite.c $(LIB_FILES) $(SRC_FILES)
