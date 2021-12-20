CC = g++
FLAGS = 

all: lexer parser generator.o symtab.o
	$(CC) $(FLAGS) parser.tab.cpp lex.yy.cpp generator.o symtab.o -o compiler

generator.o:
	$(CC) $(FLAGS) -c generator.cpp

symtab.o:
	$(CC) $(FLAGS) -c symtab.cpp

lexer:
	flex -o lex.yy.cpp lexer.l

parser:
	bison -d parser.ypp

clean:
	rm -f *.o lex.yy.cpp parser.tab.*
