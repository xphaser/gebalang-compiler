%option noyywrap
%option yylineno

%{
    #include <iostream>
    #include <string>
    #include "symtab.hpp"
    #include "generator.hpp"
    #include "parser.tab.hpp"
%}

pidentifier [_a-z]+
num         -?[0-9]+

%x COMMENT

%%
{pidentifier}   {yylval.pid = new std::string(yytext); return PIDENTIFIER;}
{num}           {yylval.num = atoll(yytext); return NUM;}
<COMMENT>[^\)]  ;
<COMMENT>\)     {BEGIN(INITIAL);}
VAR             {return VAR;}
BEGIN           {return t_BEGIN;}
END             {return END;}
ASSIGN          {return ASSIGN;}
IF              {return IF;}
THEN            {return THEN;}
ELSE            {return ELSE;}
ENDIF           {return ENDIF;}
WHILE           {return WHILE;}
DO              {return DO;}
ENDWHILE        {return ENDWHILE;}
REPEAT          {return REPEAT;}
UNTIL           {return UNTIL;}
FOR             {return FOR;}
FROM            {return FROM;}
TO              {return TO;}
DOWNTO          {return DOWNTO;}
ENDFOR          {return ENDFOR;}
READ            {return READ;}
WRITE           {return WRITE;}
PLUS            {return PLUS;}
MINUS           {return MINUS;}
TIMES           {return TIMES;}
DIV             {return DIV;}
MOD             {return MOD;}
EQ              {return EQ;}
NEQ             {return NEQ;}
LE              {return LE;}
GE              {return GE;}
LEQ             {return LEQ;}
GEQ             {return GEQ;}
\,              {return COMMA;}
\:              {return COLON;}
\;              {return SEMICOLON;}
\[              {return LBRACKET;}
\]              {return RBRACKET;}
[ \t\n\r]+      ;
\(              {BEGIN(COMMENT);}
.               {return ERROR;}
%%
