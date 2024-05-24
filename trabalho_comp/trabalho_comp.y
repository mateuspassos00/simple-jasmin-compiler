%{
#define YYSTYPE double
#include <stdio.h>
#include <stdlib.h>

int yyerror(const char *);
int yylex();

%}
%define parse.error verbose

%token TADD TMUL TSUB TDIV TAPAR TFPAR TCONSTFLOAT TCONSTINT TID TVOID TVIR TACHA TFCHA TINT TSTRING TFLOAT TRETURN TLITERAL TIF TELSE TWHILE TPVIR TMAIG TMAIOR TMEIG TMENOR TIGUAL TDIF TATRIB TAND TOR TNEG TPRINT TREAD

%%
Programa: ListaFuncoes BlocoPrincipal
	| BlocoPrincipal
	;
ListaFuncoes: ListaFuncoes Funcao
	| Funcao
	;
Funcao: TipoRetorno TID TAPAR DeclParametros TFPAR BlocoPrincipal
	| TipoRetorno TID TAPAR TFPAR BlocoPrincipal
	;
TipoRetorno: Tipo
	| TVOID
	;
DeclParametros: DeclParametros TVIR Parametro
	| Parametro
	;
Parametro: Tipo TID
	;
BlocoPrincipal: TACHA Declaracoes ListaCmd TFCHA
	| TACHA ListaCmd TFCHA
	;
Declaracoes: Declaracoes Declaracao
	| Declaracao
	;
Declaracao: Tipo ListaID TPVIR
	;
Tipo: TINT
	| TSTRING
	| TFLOAT
	;
ListaID: ListaID TVIR TID
	| TID
	;
Bloco: TACHA ListaCmd TFCHA
	;
ListaCmd: ListaCmd Comando
	| Comando
	;
Comando: CmdSe
	| CmdEnquanto
	| CmdAtribuicao
	| CmdEscrita
	| CmdLeitura
	| CmdChamadaProc
	| Retorno
	;
Retorno: TRETURN ExpressaoAritmetica TPVIR
	| TRETURN TLITERAL TPVIR
	| TRETURN TPVIR
	;
CmdSe: TIF TAPAR ExpressaoLogica TFPAR Bloco
	| TIF TAPAR ExpressaoLogica TFPAR Bloco TELSE Bloco
	;
CmdEnquanto: TWHILE TAPAR ExpressaoLogica TFPAR Bloco
	;
CmdAtribuicao: TID TATRIB ExpressaoAritmetica TPVIR
	| TID TATRIB TLITERAL TPVIR
	;
CmdEscrita: TPRINT TAPAR ExpressaoAritmetica TFPAR TPVIR
	| TPRINT TAPAR TLITERAL TFPAR TPVIR
	;
CmdLeitura: TREAD TAPAR TID TFPAR TPVIR
	;
CmdChamadaProc: ChamadaFuncao TPVIR
	;
ChamadaFuncao: TID TAPAR ListaParametros TFPAR
	| TID TAPAR TFPAR
	;
ListaParametros: ListaParametros TVIR ExpressaoAritmetica
	| ListaParametros TVIR TLITERAL
	| ExpressaoAritmetica
	| TLITERAL
	;
ExpressaoLogica: ExpressaoRelacional TAND Termo_Logico
	| ExpressaoRelacional TOR Termo_Logico
	| Termo_Logico
	;
Termo_Logico: TNEG ExpressaoLogica
	| TAPAR ExpressaoLogica TFPAR
	| ExpressaoRelacional
	;
ExpressaoRelacional: ExpressaoAritmetica TMAIG ExpressaoAritmetica
	| ExpressaoAritmetica TMAIOR ExpressaoAritmetica
	| ExpressaoAritmetica TMEIG ExpressaoAritmetica
	| ExpressaoAritmetica TMENOR ExpressaoAritmetica
	| ExpressaoAritmetica TIGUAL ExpressaoAritmetica
	| ExpressaoAritmetica TDIF ExpressaoAritmetica
	;
ExpressaoAritmetica: ExpressaoAritmetica TADD Termo
	| ExpressaoAritmetica TSUB Termo
	| Termo
	;
Termo: Termo TMUL Fator
	| Termo TDIV Fator
	| Fator
	;
Fator: TCONSTFLOAT
	| TCONSTINT
	| TID
	| ChamadaFuncao
	| TAPAR ExpressaoAritmetica TFPAR
	;
%%

int yyerror (const char *str)
{
	fprintf(stderr, "%s\n", str);
	
}
