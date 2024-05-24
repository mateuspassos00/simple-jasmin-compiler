%option noyywrap

%{
#define YYSTYPE double
#include "expr.tab.h"
#include <stdlib.h>
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
"void"	{return TVOID;}
"float"	{return TFLOAT;}
"int"	{return TINT;}
"string"	{return TSTRING;}
"return"	{return TRETURN;}
"if"	{return TIF;}
"else"	{return TELSE;}
"while"	{return TWHILE;}
"print"	{return TPRINT;}
"read"	{return TREAD;}
{const_int}	{return TCONSTINT;}
{const_float}	{return TCONSTFLOAT;}
{id}	{return TID;}
{literal}	{return TLITERAL;}
