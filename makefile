CC = g++
FLAGS = 

all: lexer parser symtab.o igen.o tgen.o
	$(CC) $(FLAGS) parser.tab.cpp lex.yy.cpp symtab.o igen.o tgen.o -o compiler

igen.o:
	$(CC) $(FLAGS) -c intermediate_generator.cpp -o igen.o

tgen.o:
	$(CC) $(FLAGS) -c target_generator.cpp -o tgen.o

symtab.o:
	$(CC) $(FLAGS) -c symtab.cpp

lexer:
	flex -o lex.yy.cpp lexer.l

parser:
	bison -d parser.ypp

clean:
	rm -f *.o lex.yy.cpp parser.tab.*
