/*
 * Parser interpretera maszyny wirtualnej do projektu z JFTT2021
 *
 * Autor: Maciek Gębala
 * http://ki.pwr.edu.pl/gebala/
 * 2021-11-12
*/
%code requires { 
#include<vector> 
#include<utility>
using namespace std;
}
%{

#define YYSTYPE long long

#include <iostream>
#include <utility>
#include <vector>

#include "instructions.hh"
#include "colors.hh"

using namespace std;

extern int yylineno;
int yylex( void );
void yyset_in( FILE * in_str );
void yyerror( vector< pair<int,int> > & program, char const *s );

%}
%parse-param { vector< pair<int,int> > & program }
%token COM_0
%token COM_1
%token JUMP_1
%token STOP
%token REG
%token LABEL
%token ERROR
%%
input :
    input line
  | %empty
  ;

line :
    COM_0	{ program.push_back( make_pair($1,0) ); }
  | COM_1 REG	{ program.push_back( make_pair($1,$2) ); }
  | JUMP_1 LABEL { program.push_back( make_pair($1,$2) ); }
  | STOP { program.push_back( make_pair($1,0) ); }
  | ERROR { yyerror( program, "Nierozpoznany symbol" ); }
  ;
%%
void yyerror( vector< pair<int,int> > & program, char const *s )
{
  cerr << cRed << "Linia " << yylineno << ": " << s << cReset << endl;
  exit(-1);
}

void run_parser( vector< pair<int,int> > & program, FILE * data ) 
{
  cout << cBlue << "Czytanie kodu." << cReset << endl;
  yyset_in( data );
  yyparse( program );
  cout << cBlue << "Skończono czytanie kodu (liczba rozkazów: " << program.size() << ")." << cReset << endl;
}
