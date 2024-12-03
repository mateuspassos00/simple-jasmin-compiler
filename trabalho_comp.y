%{
#include "arv.h"
#define YYSTYPE struct Atrib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yyerror(const char *);
int yylex();

%}
%define parse.error verbose

%token TADD TMUL TSUB TDIV TAPAR TFPAR TCONSTFLOAT TCONSTINT TID TVOID TVIR TACHA TFCHA TINT TSTRING TFLOAT TRETURN TLITERAL TIF TELSE TWHILE TPVIR TMAIG TMAIOR TMEIG TMENOR TIGUAL TDIF TATRIB TAND TOR TNEG TPRINT TREAD

%%
ProgramaLinha : Programa {gerarCodigo($1.prog); freePrograma($1.prog);}
	;
Programa: ListaFuncoes BlocoPrincipal {$$.prog = criarPrograma($1.func, $2.bloco);}
	| BlocoPrincipal {$$.prog = criarPrograma(NULL, $1.bloco);}
	;
ListaFuncoes: ListaFuncoes Funcao { 
                // Encadeia a nova função no final da lista
                struct Funcao *temp = $1.func;
                while (temp->proximo != NULL) {
                    temp = temp->proximo;
                }
                temp->proximo = $2.func;
                $2.func->anterior = temp;
                $$.func = $1.func;
            }
            | Funcao {
                $$.func = $1.func;
            }
            ;

Funcao: TipoRetorno TID TAPAR DeclParametros TFPAR BlocoPrincipal { 
            $$.func = criarFuncao(criarNoFolha(ID, $2.valorString, -1, -1), $1.arv, $4.param, $6.bloco); 
        }
       | TipoRetorno TID TAPAR TFPAR BlocoPrincipal { 
            $$.func = criarFuncao(criarNoFolha(ID, $2.valorString, -1, -1), $1.arv, NULL, $5.bloco); 
        }
       ;
TipoRetorno: Tipo {$$.arv = $1.arv;}
	| TVOID {$$.arv = criarNoFolha(TIPO, $1.valorString, -1, -1);}
	;
DeclParametros: DeclParametros TVIR Parametro {$$.param->proximo = $3.param; $3.param->anterior = $$.param;}
	| Parametro {$$.param = $1.param;}
	;
Parametro: Tipo TID {$$.param = criarParametro(criarNoFolha(ID, $2.valorString, -1, -1), $1.arv, NULL);}
	;
BlocoPrincipal: TACHA Declaracoes ListaCmd TFCHA {$$.bloco = criarBloco($2.tabelaSimb, $3.arv);}
	| TACHA ListaCmd TFCHA {$$.bloco = criarBloco(NULL, $2.arv);}
	;
Declaracoes: 
      Declaracoes Declaracao { 
          // Encadeia a nova declaração no final da lista
          struct TabelaSimb *temp = $1.tabelaSimb;
          while (temp->proximo != NULL) {
              temp = temp->proximo;
          }
          temp->proximo = $2.tabelaSimb;
          $2.tabelaSimb->anterior = temp;
          $$.tabelaSimb = $1.tabelaSimb;
      }
    | Declaracao { 
          $$.tabelaSimb = $1.tabelaSimb; 
      }
    ;

Declaracao: 
      Tipo ListaID TPVIR { 
          $$.tabelaSimb = criarTabSimb($2.arv, $1.arv); 
      }
    ;
Tipo: TINT {$$.arv = criarNoFolha(TIPO, $1.valorString, -1, -1);}
	| TSTRING {$$.arv = criarNoFolha(TIPO, $1.valorString, -1, -1);} 
	| TFLOAT {$$.arv = criarNoFolha(TIPO, $1.valorString, -1, -1);}
	;
ListaID: ListaID TVIR TID {
            struct Arv* newNode = criarNoFolha(ID, $3.valorString, -1, -1);
            struct Arv* current = $1.arv;
            while (current->proximo != NULL) {
                current = current->proximo;
            }
            current->proximo = newNode;
            newNode->anterior = current;
            $$.arv = $1.arv;
        }
        | TID {
            $$.arv = criarNoFolha(ID, $1.valorString, -1, -1);
            if ($$.arv == NULL) {
                // Handle error
            }
        }
        ;
Bloco: TACHA ListaCmd TFCHA {$$.bloco = criarBloco(NULL, $2.arv);}
	;
ListaCmd: ListaCmd Comando {
              struct Arv *current = $1.arv;
              // Percorre até o último comando da lista
              while (current->proximo != NULL) {
                  current = current->proximo;
              }
              // Atualiza o próximo do último comando para o novo comando
              current->proximo = $2.arv;
              // Atualiza o anterior do novo comando para o último comando existente
              $2.arv->anterior = current;
              // Atualiza $$ para apontar para o primeiro comando da lista
              $$.arv = $1.arv;
          }
        | Comando {
              // Se for o primeiro comando na lista, $$ recebe diretamente o comando
              $$.arv = $1.arv;
          }
        ;
Comando: CmdSe {$$.arv = $1.arv;}
	| CmdEnquanto {$$.arv = $1.arv;}
	| CmdAtribuicao {$$.arv = $1.arv;}
	| CmdEscrita {$$.arv = $1.arv;}
	| CmdLeitura {$$.arv = $1.arv;}
	| CmdChamadaProc {$$.arv = $1.arv;}
	| Retorno {$$.arv = $1.arv;}
	;
Retorno: TRETURN ExpressaoAritmetica TPVIR {$$.arv = criarOpBin(RETURN, $2.arv, NULL);}
	| TRETURN TLITERAL TPVIR {$$.arv = criarOpBin(RETURN, criarNoFolha(LITERAL, $2.valorString, -1, -1), NULL);}
	| TRETURN TPVIR {$$.arv = criarOpBin(RETURN, NULL, NULL);}
	;
CmdSe: TIF TAPAR ExpressaoLogica TFPAR Bloco {$$.arv = criarIfElse(IF, $3.arv, $5.bloco,NULL);}
	| TIF TAPAR ExpressaoLogica TFPAR Bloco TELSE Bloco {$$.arv = criarIfElse(IF, $3.arv, $5.bloco, $7.bloco);}
	;
CmdEnquanto: TWHILE TAPAR ExpressaoLogica TFPAR Bloco {$$.arv = criarWhile(WHILE, $3.arv, $5.bloco);}
	;
CmdAtribuicao: TID TATRIB ExpressaoAritmetica TPVIR {$$.arv = criarOpBin(ATRIB, criarNoFolha(ID, $1.valorString, -1, -1), $3.arv);}
	| TID TATRIB TLITERAL TPVIR {$$.arv = criarOpBin(ATRIB, criarNoFolha(ID, $1.valorString, -1, -1), criarNoFolha(LITERAL, $3.valorString, -1, -1));}
	;
CmdEscrita: TPRINT TAPAR ExpressaoAritmetica TFPAR TPVIR {$$.arv = criarEscrita(WRITE, NULL, $3.arv);}
	| TPRINT TAPAR TLITERAL TFPAR TPVIR {$$.arv = criarEscrita(WRITE, criarNoFolha(LITERAL, $3.valorString, -1, -1), NULL);}
	;
CmdLeitura: TREAD TAPAR TID TFPAR TPVIR {$$.arv = criarChamadaFuncao(READ, criarNoFolha(ID, $3.valorString, -1, -1), NULL);}
	;
CmdChamadaProc: ChamadaFuncao TPVIR {$$.arv = $1.arv;}
	;
ChamadaFuncao: TID TAPAR ListaParametros TFPAR {$$.arv = criarChamadaFuncao(FUNC, criarNoFolha(ID, $1.valorString, -1,-1), $3.param);}
	| TID TAPAR TFPAR {$$.arv = criarChamadaFuncao(FUNC, criarNoFolha(ID, $1.valorString, -1, -1), NULL);}
	;
ListaParametros: ListaParametros TVIR ExpressaoAritmetica {struct Parametro *aux = criarParametro(NULL, NULL, $3.arv); $$.param->proximo = aux; aux->anterior = $$.param;}
	| ListaParametros TVIR TLITERAL {struct Parametro *aux = criarParametro(criarNoFolha(LITERAL, $3.valorString, -1, -1), criarNoFolha(TIPO, "string", -1, -1), NULL); $$.param->proximo = aux; aux->anterior = $$.param;}
	| ExpressaoAritmetica {$$.param = criarParametro(NULL, NULL, $1.arv);}
	| TLITERAL {$$.param = criarParametro(criarNoFolha(LITERAL, $1.valorString, -1, -1), criarNoFolha(TIPO, "string", -1, -1), NULL);}
	;
ExpressaoLogica: ExpressaoRelacional TAND Termo_Logico {$$.arv = criarOpBin(AND, $1.arv, $3.arv);}
	| ExpressaoRelacional TOR Termo_Logico {$$.arv = criarOpBin(OR, $1.arv, $3.arv);}
	| Termo_Logico {$$.arv = $1.arv;}
	;
Termo_Logico: TNEG ExpressaoLogica {$$.arv = criarOpBin(NEG, $2.arv, NULL);}
	| TAPAR ExpressaoLogica TFPAR {$$.arv = $1.arv; $$.arv->parenteses = 1;}
	| ExpressaoRelacional {$$.arv = $1.arv;}
	;
ExpressaoRelacional: ExpressaoAritmetica TMAIG ExpressaoAritmetica {$$.arv = criarOpBin(MAIG, $1.arv, $3.arv);}
	| ExpressaoAritmetica TMAIOR ExpressaoAritmetica {$$.arv = criarOpBin(MAIOR, $1.arv, $3.arv);}
	| ExpressaoAritmetica TMEIG ExpressaoAritmetica {$$.arv = criarOpBin(MEIG, $1.arv, $3.arv);}
	| ExpressaoAritmetica TMENOR ExpressaoAritmetica {$$.arv = criarOpBin(MENOR, $1.arv, $3.arv);}
	| ExpressaoAritmetica TIGUAL ExpressaoAritmetica {$$.arv = criarOpBin(IGUAL, $1.arv, $3.arv);}
	| ExpressaoAritmetica TDIF ExpressaoAritmetica {$$.arv = criarOpBin(DIF, $1.arv, $3.arv);}
	;
ExpressaoAritmetica: ExpressaoAritmetica TADD Termo {$$.arv = criarOpBin(ADD, $1.arv, $3.arv);}
	| ExpressaoAritmetica TSUB Termo {$$.arv = criarOpBin(SUB, $1.arv, $3.arv);}
	| Termo {$$.arv = $1.arv;}
	;
Termo: Termo TMUL Fator {$$.arv = criarOpBin(MUL, $1.arv, $3.arv);}
	| Termo TDIV Fator {$$.arv = criarOpBin(DIV, $1.arv, $3.arv);}
	| Fator {$$.arv = $1.arv;}
	;
Fator: TCONSTFLOAT {$$.arv = criarNoFolha(FLOAT, "", -1, $1.valorDouble);}
	| TCONSTINT {$$.arv = criarNoFolha(INT, "", $1.valorInt, -1);}
	| TID {$$.arv = criarNoFolha(ID, $1.valorString, -1, -1);}
	| ChamadaFuncao {$$.arv = $1.arv;}
	| TAPAR ExpressaoAritmetica TFPAR {$$.arv = $2.arv; $2.arv->parenteses = 1;}
	;
%%

int yyerror (const char *str)
{
	fprintf(stderr, "%s\n", str);
	
}
