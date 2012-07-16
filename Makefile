SRC_FILES=src/objects.c src/nodes.c src/eval.c 

all: grammar

tests:
	mocha --reporter spec --no-colors --recursive test

grammar-debug:
	bison -y -d -t -v src/grammar.y
	flex src/lexer.l
	gcc -g -o bin/jslite y.tab.c lex.yy.c $(SRC_FILES)
	
grammar:
	bison -y -d -t -v src/grammar.y
	flex src/lexer.l
	gcc -o bin/jslite y.tab.c lex.yy.c $(SRC_FILES)
