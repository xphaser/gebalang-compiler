all: lexer parser
	g++ parser.tab.cpp lex.yy.cpp -o compiler

lexer:
	flex -o lex.yy.cpp lex.l

parser:
	bison -d parser.ypp

clean:
	rm lex.yy.cpp parser.tab.*
