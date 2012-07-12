all: grammar
	
grammar:
	bison -y -d -t --verbose src/grammar.y
	flex src/lexer.l
	gcc -o bin/jslite y.tab.c lex.yy.c
	rm y.tab.c lex.yy.c y.tab.h
