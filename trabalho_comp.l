%option noyywrap

%{
#include "arv.h"
#define YYSTYPE struct Atrib
#include "trabalho_comp.tab.h"
#include <stdlib.h>
#include <string.h>
%}

delim	[ \n\t]
ws	{delim}+
digito	[0-9]
const_int {digito}+
const_float	{digito}+(\.{digito}*(E[+-]?{digito}+)?)?
letra	[A-Za-z]
id	{letra}[_A-Za-z0-9]*
literal \"[{letra}_{digito}]*\"

%%
{ws}	{}
"+"	{return TADD;}
"-"	{return TSUB;}
"*"	{return TMUL;}
"/"	{return TDIV;}
">="	{return TMAIG;}
">"	{return TMAIOR;}
"<="	{return TMEIG;}
"<"	{return TMENOR;}
"=="	{return TIGUAL;}
"!="	{return TDIF;}
"&&"	{return TAND;}
"||"	{return TOR;}
"!"	{return TNEG;}
"="	{return TATRIB;}
"("	{return TAPAR;}
")"	{return TFPAR;}
"{"	{return TACHA;}
"}"	{return TFCHA;}
";"	{return TPVIR;}
","	{return TVIR;}
"void"	{strcpy(yylval.valorString, yytext); return TVOID;}
"float"	{strcpy(yylval.valorString, yytext); return TFLOAT;}
"int"	{strcpy(yylval.valorString, yytext); return TINT;}
"string"	{strcpy(yylval.valorString, yytext); return TSTRING;}
"return"	{return TRETURN;}
"if"	{return TIF;}
"else"	{return TELSE;}
"while"	{return TWHILE;}
"print"	{return TPRINT;}
"read"	{return TREAD;}
{const_int}	{yylval.valorInt = atoi(yytext); return TCONSTINT;}
{const_float}	{yylval.valorDouble = atof(yytext); return TCONSTFLOAT;}
{id}	{strcpy(yylval.valorString, yytext); return TID;}
{literal}	{strcpy(yylval.valorString, yytext); return TLITERAL;}
