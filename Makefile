SRC_FILES=src/nodes.c src/eval.c src/runtime.c  
LIB_FILES=lib/console.c

all: grammar

tests:
	mocha --reporter spec --no-colors --recursive test

grammar-debug:
	bison -y -d -t -v src/grammar.y
	flex src/lexer.l
	gcc -g -o bin/jslite y.tab.c lex.yy.c src/jslite.c $(LIB_FILES) $(SRC_FILES)
	
grammar:
	bison -y -d -t -v src/grammar.y
	flex src/lexer.l
	gcc -o bin/jslite y.tab.c lex.yy.c src/jslite.c $(LIB_FILES) $(SRC_FILES) 
