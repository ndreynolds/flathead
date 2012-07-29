# Makefile
# --------
# jslite 

COMPILER=gcc
YACC=bison -y -d -t -v
LEX=flex

SRC_FILES=src/nodes.c src/eval.c src/runtime.c  
LIB_FILES=lib/console.c
OUT_FILE=-o bin/jslite
GRAMMAR_FILE=src/grammar.y
LEX_FILE=src/lexer.l

all: clean default

tests:
	mocha --reporter spec --no-colors --recursive test

grammar:
	$(YACC) $(GRAMMAR_FILE)

lexer:
	$(LEX) $(LEX_FILE)

clean:
	rm -rf y.* lex.yy.c bin/jslite* a.out

debug: grammar lexer
	$(COMPILER) -g $(OUT_FILE) y.tab.c lex.yy.c src/jslite.c $(LIB_FILES) $(SRC_FILES)
	
default: grammar lexer
	$(COMPILER) $(OUT_FILE) y.tab.c lex.yy.c src/jslite.c $(LIB_FILES) $(SRC_FILES)
