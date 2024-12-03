#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arv.h"

/* A FAZER
*
* Distinguir entre warnings e erros, fazer os erros não gerarem bytecodes;
* Passar o pente fino na verificação de contexto;
* Gerar bytecode somente para tipo inteiro por conta do tempo (eventualmente implementar para string e float);
*
*/

/**********************************************************************************************************************
********************************************** ALOCAÇÃO DE DADOS ******************************************************
**********************************************************************************************************************/

struct Programa *criarPrograma(struct Funcao *funcoes, struct Bloco *blocoPrincipal) {
    struct Programa *prog = malloc(sizeof(*prog));
    if(!prog) {perror("malloc_fail"); abort();}
    prog->funcoes = funcoes;
    prog->blocoPrincipal = blocoPrincipal;

    return prog;
}

struct Funcao *criarFuncao(struct Arv *nome, struct Arv *tipoRetorno, struct Parametro *parametros, struct Bloco *blocoPrincipal) {
    struct Funcao *funcao = malloc(sizeof(*funcao));
    if(!funcao) {perror("malloc_fail"); abort();}
    funcao->nome = nome;
    funcao->tipoRetorno = tipoRetorno;
    funcao->declPar = parametros;
    funcao->blocoPrincipal = blocoPrincipal;

    return funcao;
}

struct Parametro *criarParametro(struct Arv *id, struct Arv *tipo, struct Arv* exprAritm) {
    struct Parametro *p = malloc(sizeof(*p));
    if(!p) {perror("malloc_fail"); abort();}
    p->nome = id;
    p->tipo = tipo;
    p->exprAritm = exprAritm;

    return p;
}

struct TabelaSimb *criarTabSimb(struct Arv *listaIDs, struct Arv *tipo) {
/* Recebe uma lista de IDs e cria vários nós do tipo TabelaSimb para acomodar cada variável
*  Todos os IDs compartilham o mesmo tipo e, provavelmente, o mesmo local (como estão no mesmo
*  escopo, provavelmente vão estar no mesmo frame na pilha... vamos ver na etapa 4)
*/
    // Nó do primeiro ID na lista
    struct TabelaSimb *tab = malloc(sizeof(*tab));
        if(!tab) {perror("malloc_fail"); abort();}
        
        tab->id = listaIDs;        
        tab->tipo = tipo;
        
        // Verifica se existem mais IDs e vai criando e encadeando os nós
        struct TabelaSimb *aux2 = tab;
        struct Arv *aux = tab->id;
        while(aux2->id->proximo) {
            aux = aux2->id->proximo;
            struct TabelaSimb *tmp = malloc(sizeof(*tmp));

            tmp->id = aux;
            tmp->tipo = aux2->tipo;
            tmp->local = aux2->local;
            tmp->anterior = aux2;
            aux2->proximo = tmp;
            aux2 = aux2->proximo;
        }
    return tab;
    
}

struct Bloco *criarBloco(struct TabelaSimb *declaracoes, struct Arv *listaCmds) {
    struct Bloco *bloco = malloc(sizeof(*bloco));
    if(!bloco) {perror("malloc_fail"); abort();}
    bloco->listaCmds = listaCmds;
    bloco->declaracoes = declaracoes;

    return bloco;
}

struct Arv *criarWhile(enum op op, struct Arv *exprLog, struct Bloco *bloco) {
    struct Arv *noWhile = malloc(sizeof(*noWhile));
    if(!noWhile) {perror("malloc_fail"); abort();}
    noWhile->op = op;
    noWhile->arv1 = exprLog;
    noWhile->bloco1 = bloco;

    return noWhile;
}

struct Arv *criarIfElse(enum op op, struct Arv *exprLog, struct Bloco *b1, struct Bloco *b2) {
    struct Arv *noIfElse = malloc(sizeof(*noIfElse));
    if(!noIfElse) {perror("malloc_fail"); abort();}
    noIfElse->op = op;
    noIfElse->arv1 = exprLog;
    noIfElse->bloco1 = b1;
    noIfElse->bloco2 = b2;

    return noIfElse;
}

struct Arv *criarOpBin(enum op op, struct Arv *no1, struct Arv *no2) {
    struct Arv *noOpBin = malloc(sizeof(*noOpBin));
    if(!noOpBin) {perror("malloc_fail"); abort();}
    noOpBin->op = op;
    noOpBin->arv1 = no1;
    noOpBin->arv2 = no2;

    return noOpBin;
}

struct Arv *criarChamadaFuncao(enum op op, struct Arv *nome, struct Parametro *listaParam) {
    struct Arv *noChamada = malloc(sizeof(*noChamada));
    if(!noChamada) {perror("malloc_fail"); abort();}
    noChamada->op = op;
    noChamada->arv1 = nome;
    noChamada->listaParam = listaParam;

    return noChamada;
}

struct Arv *criarEscrita(enum op op, struct Arv* literal, struct Arv* exprAritm) {
    struct Arv *noEscrita = malloc(sizeof(*noEscrita));
    if(!noEscrita) {perror("malloc_fail"); abort();}
    noEscrita->op = op;
    noEscrita->arv1 = exprAritm;
    noEscrita->arv2 = literal;

    return noEscrita;
}

struct Arv *criarNoFolha(enum op op, const char *nome, int valorInt, double valorDouble) {
    struct Arv *noFolha = malloc(sizeof(*noFolha));
    if(!noFolha) {perror("malloc_fail"); abort();}
    noFolha->op = op;
    strcpy(noFolha->valorString, nome);
    noFolha->valorInt = valorInt;
    noFolha->valorDouble = valorDouble;

    return noFolha;
}

/*********************************************************************************************************
***************************************** PRINTAR O CÓDIGO ***********************************************
*********************************************************************************************************/
void printBloco(struct Bloco *bloco);
void printIfElse(struct Arv* cmd) ;
void printListaParam(struct Parametro* par) ;
void printDeclarParam(struct Parametro *par);
void printFuncoes(struct Funcao *funcoes);
void printExprRel(struct Arv* exprRel);
void printChamadaFuncao(struct Arv* exprAritm);
void printExprAritm(struct Arv* exprAritm);

void printDeclaracoes(struct TabelaSimb* decl) {
    struct TabelaSimb *tmp = decl;
    do{        
        printf("%s ", tmp->tipo->valorString); // printa o tipo da lista de variáveis de mesmo tipo        
        struct TabelaSimb *tmp2 = tmp;
        if(tmp2->id->proximo) {            
            while(!strcmp(tmp2->proximo->tipo->valorString, tmp2->tipo->valorString)) {
                // enquanto os tipos de uma variável e da próxima forem iguais, vai printando tudo em sequência
                // separado por vírgula
                printf("%s, ", tmp2->id->valorString);
                tmp2 = tmp2->proximo;
                if(!tmp2->proximo) break;
            }            
        }

        printf("%s;\n", tmp2->id->valorString); // printa a última variável da lista de variáveis que compartilham tipo
        
        tmp = tmp2->proximo;
    }while(tmp);

    printf("\n");

    return;
}
void printDeclarParam(struct Parametro *par) {
    struct Parametro *tmp = par;
    printf("%s %s", tmp->tipo->valorString, tmp->nome->valorString);
    if (tmp->proximo) {
    do {
        tmp = tmp->proximo;
        printf(", %s %s", tmp->tipo->valorString, tmp->nome->valorString);
    } while (tmp->proximo);
}

    return;
}

void printFuncoes(struct Funcao *funcoes) {
    struct Funcao *tmp = funcoes;
    do{
        printf("%s %s(", tmp->tipoRetorno->valorString, tmp->nome->valorString);
        if(tmp->declPar) printDeclarParam(tmp->declPar);
        printf(")\n");
        printBloco(tmp->blocoPrincipal);

        tmp = tmp->proximo;
    }while(tmp);
    
    return;
}

void printExprLog(struct Arv* exprLog) {
    if(exprLog->parenteses) printf("(");

    switch(exprLog->op) {
        case AND:
            printExprRel(exprLog->arv1);
            printf(" && ");
            if(exprLog->arv2->op == NEG || exprLog->arv2->parenteses) printExprLog(exprLog->arv2);
            else printExprRel(exprLog->arv2);
            break;
        case OR:
            printExprRel(exprLog->arv1);
            printf(" || ");
            if(exprLog->arv2->op == NEG || exprLog->arv2->parenteses) printExprLog(exprLog->arv2);
            else printExprRel(exprLog->arv2);
            break;
        case NEG:
            printf("!"); 
            printExprLog(exprLog->arv1);
            break;
        default:
            printExprRel(exprLog);
            break;
    }

    if(exprLog->parenteses) printf(")");
}

void printExprRel(struct Arv* exprRel) {
    printExprAritm(exprRel->arv1);
    switch(exprRel->op) {
        case MAIG:
            printf(" >= ");
            break;
        case MAIOR:
            printf(" > ");
            break;
        case MEIG:
            printf(" <= ");
            break;
        case MENOR:
            printf(" < ");
            break;
        case IGUAL:
            printf(" == ");
            break;
        case DIF:
            printf(" != ");
            break;
    }
    printExprAritm(exprRel->arv2);

    return;
}

void printChamadaFuncao(struct Arv* exprAritm) {
    printf("%s(",exprAritm->arv1->valorString);
    if(exprAritm->listaParam) printListaParam(exprAritm->listaParam);
    printf(")");

    return;
}

void printExprAritm(struct Arv* exprAritm) {
    if(exprAritm->parenteses) printf("(");

    if(exprAritm->op < 4) {
        printExprAritm(exprAritm->arv1);
        switch(exprAritm->op) {
            case ADD:
                printf(" + ");
                break;
            case SUB:
                printf(" - ");
                break;
            case MUL:
                printf(" * ");
                break;
            case DIV:
                printf(" / ");
                break;
        }
        printExprAritm(exprAritm->arv2);
    }
    
    else {
        switch(exprAritm->op) {
            case FUNC:
                printChamadaFuncao(exprAritm);
                break;
            case ID:
                printf("%s", exprAritm->valorString);
                break;
            case INT:
                printf("%d", exprAritm->valorInt);
                break;
            case FLOAT:
                printf("%f", exprAritm->valorDouble);
                break;
        }
    }

    if(exprAritm->parenteses) printf(")");    

    return;
}

void printReturn(struct Arv* cmd) {
    if(!cmd->arv1) printf("return;\n");
    else if(cmd->arv1->op == LITERAL) printf("return %s;\n", cmd->arv1->valorString);
    else {
        printf("return ");
        printExprAritm(cmd->arv1);
        printf(";\n");
    }

    return;
}

void printIfElse(struct Arv* cmd) {
    printf("if(");
    printExprLog(cmd->arv1);
    printf(") ");
    printBloco(cmd->bloco1);
    if(cmd->bloco2) {
        printf("else ");
        printBloco(cmd->bloco2);
    }

    return;
}

void printWhile(struct Arv* cmd) {
    printf("while (");
    printExprLog(cmd->arv1);
    printf(") ");
    printBloco(cmd->bloco1);

    return;
}

void printAtrib(struct Arv* cmd) {
    if(cmd->arv2->op == LITERAL) printf("%s = %s;\n", cmd->arv1->valorString, cmd->arv2->valorString);
    else if(cmd->arv2->op == FUNC){
        printf("%s = ", cmd->arv1->valorString);
        printChamadaFuncao(cmd->arv2);
        printf(";\n");
    }
    else{
        printf("%s = ", cmd->arv1->valorString);
        printExprAritm(cmd->arv2);
        printf(";\n");
    }//O analisador semântico deve receber como entrada a AST,

    return;
}

void printWrite(struct Arv* cmd) {
    if(cmd->arv1 && cmd->arv1->op == LITERAL) printf("print(%s);\n", cmd->arv1->valorString);
    else {
        printf("print(");
        printExprAritm(cmd->arv2);
        printf(");\n");
    }

    return;
}

void printRead(struct Arv* cmd) {
    printf("read(%s);\n", cmd->arv1->valorString);
    
    return;
}

void printListaParam(struct Parametro* par) {
    struct Parametro *tmp = par;
    while (tmp) {
        if (tmp->exprAritm) {
            printExprAritm(tmp->exprAritm);
        } else {
            printf("%s", tmp->nome->valorString);
        }
        
        if (tmp->proximo) {
            printf(", ");
        }
        
        tmp = tmp->proximo;
    }
}

void printCmdChamadaFunc(struct Arv* cmd) {
    printf("%s(",cmd->arv1->valorString);
    if(cmd->listaParam) printListaParam(cmd->listaParam);
    printf(");\n");

    return;
}

void printListaCmds(struct Arv* listaCmds) {
    struct Arv* tmp = listaCmds;
    do {
        switch(tmp->op) {
            case IF: printIfElse(tmp); break;
            case WHILE: printWhile(tmp); break;
            case ATRIB: printAtrib(tmp); break;
            case WRITE: printWrite(tmp); break;
            case READ: printRead(tmp); break;
            case FUNC: printCmdChamadaFunc(tmp); break;
            case RETURN: printReturn(tmp); break;
        }
        tmp = tmp->proximo;
    }while(tmp);

    return;
}
void printBloco(struct Bloco *bloco) {
    printf("{\n");
    if(bloco->declaracoes) printDeclaracoes(bloco->declaracoes);
    printListaCmds(bloco->listaCmds);
    printf("}\n");

    return;
}

void printCodigo(struct Programa *prog) {
    // Todo o processo de printar o código é facilmente guiado pela gramática
    if(!prog) return;
    if(prog->funcoes) printFuncoes(prog->funcoes);
    printBloco(prog->blocoPrincipal);
    return;
}


/**********************************************************************************************************************
******************************************* VERIFICAÇÃO DE CONTEXTO ***************************************************
**********************************************************************************************************************/

void verificarTipos(struct Arv *);
void verificarChamadaFuncao(struct Arv *, struct Funcao *);
void verificarAtribuicao(struct Arv *, struct TabelaSimb *,struct Funcao *,struct Funcao *);
void verificarRetornoFuncao(struct Arv *, struct Funcao *,struct TabelaSimb *);

int erro = 0;

enum op stringParaTipo(const char *tipoString) {
    if(!tipoString){
        return LITERAL;
    }
    if (strcmp(tipoString, "int") == 0) {
        return INT;
    } else if (strcmp(tipoString, "float") == 0) {
        return FLOAT;
    } else if (strcmp(tipoString, "string") == 0) {
        return STRING;
    }
}
struct TabelaSimb *buscarVariavel(const char *nomeVariavel, struct TabelaSimb *tabelaSimbolos) {
    struct TabelaSimb *temp = tabelaSimbolos;

    while (temp != NULL) {
        if (strcmp(temp->id->valorString, nomeVariavel) == 0) {
            return temp; // Encontrou a variável
        }
        temp = temp->proximo;
    }

    return NULL; // Não encontrou a variável
}
const char* opToString(enum op operador) {
    switch (operador) {
        case ID:
            return "id";
        case INT:
            return "int";
        case FLOAT:
            return "float";
        case STRING:
            return "string";
        case ATRIB:
            return "=";
        case ADD:
            return "+";
        case SUB:
            return "-";
        case MUL:
            return "*";
        case DIV:
            return "/";
        // Adicione outros operadores conforme necessário
        default:
            return "Operador desconhecido";
    }
}
char* determinarTipoExpressao(struct Arv *expressao, struct TabelaSimb *declaracoes, struct Funcao *funcoes, struct Funcao *funcaoAtual) {
    if (expressao == NULL) {
        return NULL;
    }

    switch (expressao->op) {
        case ID:
            {
                struct TabelaSimb *variavel = buscarVariavel(expressao->valorString, declaracoes);

                if (variavel != NULL) {
                    return variavel->tipo->valorString;
                } else {
                    if(funcaoAtual){
                    struct Parametro *parametro = funcaoAtual->declPar;
                    int encontrado = 0;
                    while (parametro != NULL) {
                    if (strcmp(parametro->nome->valorString, expressao->valorString) == 0) {
                        encontrado = 1;
                        break;
                    }
                    parametro = parametro->proximo;
                    }
                    if(encontrado >= 1){
                        return parametro->tipo->valorString;
                    }else{
                        printf("Erro semântico: Variável '%s' não declarada.\n", expressao->valorString);
                        erro = 1;
                        return NULL;
                    }
                }
                    printf("Erro semântico: Variável '%s' não declarada.\n", expressao->valorString);
                    erro = 1;
                    return NULL;
                }
            }
        case INT:
        case FLOAT:
        case LITERAL:
            return opToString(expressao->op);
        case ADD:
        case SUB:
        case MUL:
        case DIV:
            {
                char *tipoEsq = determinarTipoExpressao(expressao->arv1, declaracoes,funcoes,funcaoAtual);
                char *tipoDir = determinarTipoExpressao(expressao->arv2, declaracoes,funcoes,funcaoAtual);

                if (tipoEsq != NULL && tipoDir != NULL) {
                    if (strcmp(tipoEsq, tipoDir) == 0) {
                        return tipoEsq;
                    } else {
                        printf("Warning semântico: Tipos incompatíveis na expressão '%s %s %s'.\n", tipoEsq, opToString(expressao->op), tipoDir);
                        return tipoDir;
                    }
                }
                return NULL;
            }
        case FUNC:
            {
            struct Funcao *funcao = funcoes;
            while (funcao != NULL && strcmp(funcao->nome->valorString, expressao->arv1->valorString) != 0) {
                funcao = funcao->proximo;
            }
            return funcao->tipoRetorno->valorString;
            }
        default:
            printf("Erro semântico: Operador desconhecido.\n");
            erro = 1;
            return NULL;
    }
}

int contarParametros(struct Parametro *parametros) {
    int count = 0;
    struct Parametro *param = parametros;
    while (param != NULL) {
        count++;
        param = param->proximo;
    }
    return count;
}


void verificarQuantidadeParametrosRecursivo(struct Arv *arv, struct Funcao *funcoes) {
    if (arv == NULL) {
        return;
    }

    // Verificar nó atual
    if (arv->op == FUNC) {
        // Procurar a função na lista de funções
        struct Funcao *funcao = funcoes;
        while (funcao != NULL && strcmp(funcao->nome->valorString, arv->arv1->valorString) != 0) {
            funcao = funcao->proximo;
        }

        // Verificar se a função foi declarada
        if (funcao == NULL) {
            printf("Erro: Chamada para função não declarada %s.\n", arv->arv1->valorString);
            erro = 1;
            return;
        }

        // Contar número de parâmetros na chamada
        int numParametrosChamada = contarParametros(arv->listaParam);
        int numParametrosDeclarados = contarParametros(funcao->declPar);

        // Verificar se a quantidade de parâmetros está correta
        if (numParametrosChamada != numParametrosDeclarados) {
            printf("Erro: Número incorreto de parâmetros na chamada para função %s. Esperado: %d, Encontrado: %d\n",
                   arv->arv1->valorString, numParametrosDeclarados, numParametrosChamada);
        }
        struct Parametro *param = arv->listaParam;
        struct Parametro *declParam = funcao->declPar;
        while (param != NULL && declParam != NULL) {
            if(param->exprAritm->op == stringParaTipo(declParam->tipo->valorString)){
                param = param->proximo;
                declParam = declParam->proximo;
            }else{
                printf("Erro: Tipos diferentes de parametros na chamada %s, esperava %s e recebeu %s\n", arv->arv1->valorString,declParam->tipo->valorString,opToString(param->exprAritm->op));
                erro = 1;
                break;
            }
        }
    }

    // Verificar nos filhos da árvore recursivamente
    verificarQuantidadeParametrosRecursivo(arv->arv1, funcoes);
    verificarQuantidadeParametrosRecursivo(arv->arv2, funcoes);
}

void verificarQuantidadeParametros(struct Arv *arv, struct Funcao *funcoes) {
    if (arv == NULL) {
        return;
    }

    // Chamada inicial para verificar recursivamente
    verificarQuantidadeParametrosRecursivo(arv, funcoes);
}

struct Bloco *verificarTiposTab(struct Bloco *bloco, struct Funcao *funcoes, struct Funcao *funcaoAtual,struct TabelaSimb *declaracaoBP) {
    if (bloco == NULL) {
        return NULL;
    }

    // Verificar tipos nas declarações de variáveis
    struct TabelaSimb *declaracao = bloco->declaracoes;
    while (declaracao != NULL) {
        // Verifique se a variável foi declarada mais de uma vez
        struct TabelaSimb *outraDeclaracao = bloco->declaracoes;
        while (outraDeclaracao != declaracao) {
            if (strcmp(declaracao->id->valorString, outraDeclaracao->id->valorString) == 0) {
                printf("Aviso: Variável %s declarada mais de uma vez.\n", declaracao->id->valorString);
            }
            outraDeclaracao = outraDeclaracao->proximo;
        }
        declaracao = declaracao->proximo;
    }

    // Verificar os comandos dentro do bloco
    struct Arv *comando = bloco->listaCmds;
    struct TabelaSimb *Declaracao;
    if(funcaoAtual){
        Declaracao = funcaoAtual->blocoPrincipal->declaracoes;
    }else{
        Declaracao = bloco->declaracoes;
    }
    if(!Declaracao){
        Declaracao = declaracaoBP;
    }
    while (comando != NULL) {
        verificarTipos(comando);
        verificarChamadaFuncao(comando, funcoes);
        if(comando->op == IF || WHILE){
            if(comando->bloco1){
                verificarTiposTab(comando->bloco1,funcoes,funcaoAtual,Declaracao);
            }
            if(comando->bloco2){
                verificarTiposTab(comando->bloco2,funcoes,funcaoAtual,Declaracao);
            }
        }
        verificarAtribuicao(comando,Declaracao,funcoes,funcaoAtual); // Passa a tabela de símbolos do bloco atual
        verificarRetornoFuncao(comando, funcaoAtual,Declaracao); // Passa a função atual para verificar o retorno
        verificarQuantidadeParametros(comando,funcoes);
        comando = comando->proximo;
    }

    return bloco;
}


void verificarTipos(struct Arv *arv) {
    if (arv == NULL) {
        return;
    }

    // Verifica os tipos dos nós filhos
    verificarTipos(arv->arv1);
    verificarTipos(arv->arv2);

    // Verifica as regras de coerção de tipos
    if (arv->op == ADD || arv->op == SUB || arv->op == MUL || arv->op == DIV) {
        if (arv->arv1->op == INT && arv->arv2->op == FLOAT) {
            // Converte o operando do tipo int para double
            arv->arv1->op = FLOAT;
            arv->arv1->valorDouble = (float)arv->arv1->valorInt;
        } else if (arv->arv1->op == FLOAT && arv->arv2->op == INT) {
            // Converte o operando do tipo int para double
            arv->arv2->op = FLOAT;
            arv->arv2->valorDouble = (float)arv->arv2->valorInt;
        }
    }

    // Adicione mais regras de verificação de tipos conforme necessário
}

void verificarChamadaFuncao(struct Arv *arv, struct Funcao *funcoes) {
    if (arv == NULL || arv->op != FUNC) {
        return;
    }

    // Procurar a função na lista de funções
    struct Funcao *funcao = funcoes;
    while (funcao != NULL && strcmp(funcao->nome->valorString, arv->arv1->valorString) != 0) {
        funcao = funcao->proximo;
    }

    // Verificar se a função foi declarada
    if (funcao == NULL) {
        printf("Erro: Chamada para função não declarada %s.\n", arv->valorString);
        erro = 1;
        return;
    }

    // Verificar o número de parâmetros
    struct Parametro *param = arv->listaParam;
    struct Parametro *declParam = funcao->declPar;
    while (param != NULL && declParam != NULL) {
        param = param->proximo;
        declParam = declParam->proximo;
    }

    // Verificar os tipos dos parâmetros
    param = arv->listaParam;
    declParam = funcao->declPar;
    while (param != NULL && declParam != NULL) {        
        // Comparar tipos dos parâmetros
        enum op tipoParamDeclarado = stringParaTipo(declParam->tipo->valorString);
        enum op tipoParamPassado = stringParaTipo(determinarTipoExpressao(param->exprAritm, NULL,NULL,NULL)); // Passa NULL para não verificar variáveis no contexto atual

        if (tipoParamPassado != tipoParamDeclarado) {
            printf("Erro: Tipo de parâmetro %s incompatível na chamada para função %s.\n", opToString(tipoParamDeclarado), arv->arv1->valorString);
            erro = 1;
        }        

        param = param->proximo;
        declParam = declParam->proximo;
    }
}
void verificarAtribuicao(struct Arv *comando, struct TabelaSimb *declaracoes,struct Funcao *funcoes ,struct Funcao *funcaoAtual) {
    if (comando == NULL) {
        return;
    }

    // Verifica atribuição apenas se o nó for do tipo ATRIB
    if (comando->op == ATRIB) {
        // Verificar se o nó da atribuição possui dois filhos
        if (comando->arv1 == NULL || comando->arv2 == NULL) {
            printf("Erro semântico: Atribuição mal formada.\n");
            erro = 1;
            return;
        }

        // Encontrar a variável na tabela de símbolos
        struct TabelaSimb *variavel = buscarVariavel(comando->arv1->valorString, declaracoes);

        if (variavel == NULL) {
            // Verifica se a variável é um parâmetro da função atual
            if(funcaoAtual){            
                struct Parametro *parametro = funcaoAtual->declPar;
                int encontrado = 0;
                while (parametro != NULL) {
                    if (strcmp(parametro->nome->valorString, comando->arv1->valorString) == 0) {
                        encontrado = 1;
                        break;
                    }
                    parametro = parametro->proximo;
                }

                if (!encontrado) {
                    printf("Aviso: Variável '%s' não é um parâmetro da função atual.\n", comando->arv1->valorString);
                    printf("Erro semântico: Variável '%s' não declarada.\n", comando->arv1->valorString);
                    erro = 1;
                }
            
            }
            
            else printf("Erro semântico: Variável '%s' não declarada.\n", comando->arv1->valorString);            
            erro = 1;
        } else if(comando->arv2->op== stringParaTipo(variavel->tipo->valorString)){}
        
        else {
 // Determinar o tipo da expressão no lado direito da atribuição
            char *tipoExpressao = determinarTipoExpressao(comando->arv2, declaracoes,funcoes,funcaoAtual);

            // Comparar os tipos usando valorString
            if (tipoExpressao != NULL && strcmp(variavel->tipo->valorString, tipoExpressao) != 0) {
                // Verificar a conversão necessária de tipos
                if ((strcmp(variavel->tipo->valorString, "int") == 0 && strcmp(tipoExpressao, "float") == 0) ||
                    (strcmp(variavel->tipo->valorString, "float") == 0 && strcmp(tipoExpressao, "int") == 0)) {
                    // Realizar a conversão
                    if (strcmp(variavel->tipo->valorString, "int") == 0) {
                        // Converte o tipo de arv2 de float para int
                        comando->arv2->op = INT;
                        comando->arv2->valorInt = (int)comando->arv2->valorDouble;
                        printf("Aviso: Conversão de tipo realizada para atribuição à variável '%s'.\n", comando->arv1->valorString);
                    } else { // variavel->tipo->valorString == "float"
                        // Converte o tipo de arv2 de int para float
                        comando->arv2->valorDouble = (float)comando->arv2->valorInt;
                        printf("Aviso: Conversão de tipo realizada para atribuição à variável '%s'.\n", comando->arv1->valorString);
                    }
                } else {
                    printf("Erro semântico: Atribuição incompatível para variável '%s'.\n", comando->arv1->valorString);
                    erro = 1;
                }
            }
        }
    }

    // Verificar nos filhos da árvore
    verificarAtribuicao(comando->arv1, declaracoes,funcoes,funcaoAtual);
    verificarAtribuicao(comando->arv2, declaracoes,funcoes,funcaoAtual);
}
void verificarRetornoFuncao(struct Arv *arv, struct Funcao *funcaoAtual,struct TabelaSimb *declaracoes) {
    // Verifica se a função atual tem tipo de retorno void
    if (funcaoAtual == NULL || funcaoAtual->tipoRetorno == NULL || strcmp(funcaoAtual->tipoRetorno->valorString, "void") == 0) {
        // Se for void, o retorno não deve conter expressão
        if (arv != NULL && arv->op == RETURN && arv->arv1 != NULL) {
            fprintf(stderr, "Erro: Função void não pode ter retorno com expressão.\n");erro = 1;
            // Você pode adicionar mais informações ou lógica de tratamento de erro conforme necessário
        }
    } else {
        // Verifica se há retorno com expressão e compara com o tipo de retorno da função
        if (arv != NULL && arv->op == RETURN) {
            // Verifica se a expressão de retorno existe
            if (arv->arv1 == NULL) {
                fprintf(stderr, "Erro: Função %s requer retorno com expressão.\n", funcaoAtual->nome->valorString);erro = 1;
                // Você pode adicionar mais informações ou lógica de tratamento de erro conforme necessário
            } else {
                // Verifica se os tipos são compatíveis
                enum op tipoRetornoFuncao = stringParaTipo(funcaoAtual->tipoRetorno->valorString);
                enum op tipoExprRetorno;
                if(arv->arv1->op != FLOAT && arv->arv1->op !=INT && arv->arv1->op !=STRING && arv->arv1->op !=ID){
                    // retorno não é tipo primitivo ou variável, deve-se terminar o tipo
                    tipoExprRetorno = stringParaTipo(determinarTipoExpressao(arv->arv1, declaracoes, NULL, funcaoAtual));
                }else{
                    tipoExprRetorno = stringParaTipo(buscarVariavel(arv->arv1->valorString, declaracoes)->tipo->valorString);
                }

                if (tipoRetornoFuncao != tipoExprRetorno) {
                    printf("Erro: Tipo de retorno incompatível com a função %s.\n", funcaoAtual->nome->valorString);erro = 1;
                    // Você pode adicionar mais informações ou lógica de tratamento de erro conforme necessário
                }
            }
        }
    }
}


struct Programa *verificarSemantica(struct Programa *programa) {
    // Verificar tipos nas funções
    struct Funcao *funcao = programa->funcoes;
    while (funcao != NULL) {
        funcao->blocoPrincipal = verificarTiposTab(funcao->blocoPrincipal, programa->funcoes, funcao,NULL);
        funcao = funcao->proximo;
    }

    // Verificar tipos no bloco principal
    programa->blocoPrincipal = verificarTiposTab(programa->blocoPrincipal, programa->funcoes, NULL,NULL);

    return programa;
}

/**********************************************************************************************************************
******************************************* GERANDO INSTRUÇÕES DA JVM *************************************************
**********************************************************************************************************************/

int numLabel = 0;

void gerarCodigoBloco(struct Bloco *, struct Parametro *, int , FILE *);
void gerarCodigoExprAritm(struct Arv *, FILE *);

void setLocais(struct Arv *cmd, struct TabelaSimb *var, struct Parametro *par) {    
    if(!var && !par) return;
    
    if(cmd->op == ID) {            
        if(var) {
            struct TabelaSimb *tmp = var;
            while(tmp) {
                if(!strcmp(tmp->id->valorString, cmd->valorString)) cmd->local = tmp->local;
                tmp = tmp->proximo;
            }
        }

        if(par) {
            struct Parametro *tmp = par;
            while(tmp) {
                if(!strcmp(tmp->nome->valorString, cmd->valorString)) cmd->local = tmp->local;
                tmp = tmp->proximo;
            }
        }
    }
    
    else{
        if(cmd->arv1) setLocais(cmd->arv1, var, par);
        if(cmd->arv2) setLocais(cmd->arv2, var, par);
        if(cmd->bloco1) setLocais(cmd->bloco1->listaCmds, var, par);
        if(cmd->bloco2) setLocais(cmd->bloco2->listaCmds, var, par);
        if(cmd->listaParam) {
            struct Parametro *tmp = cmd->listaParam;
            while(tmp) {
                setLocais(tmp->exprAritm, var, par);
                tmp = tmp->proximo;
            }
        }
    }

    return;
}

char *gerarLabel() {
    char *label = malloc(5 * sizeof(char));

    snprintf(label, 5, "l%d", numLabel);
    numLabel++;

    return label;
}

void gerarCodigoInteiro(int valor, FILE *out) {
    if(valor >=0 && valor < 5) {
        char num[2], aux[10] = "\ticonst_";

        snprintf(num, sizeof(num), "%d", valor);
        strcat(aux, num);

        fprintf(out, "%s\n", aux);
    }

    else if(valor > -127 && valor < 127) {
        fprintf(out, "\tbipush %d\n", valor);
    }

    else fprintf(out, "\tldc %d\n", valor);

    return;
}

char *gerarListaParam(struct Parametro *listaParam) {
    struct Parametro *tmp = listaParam;
    
    char *listaTipos = malloc(sizeof(char));
    strcpy(listaTipos, "");
    int tam = 0;
    while(tmp) {
        tam++;
        listaTipos = realloc(listaTipos, (tam + 1) * sizeof(char));
        strcat(listaTipos, "I");
        tmp = tmp->proximo;
    }

    return listaTipos;   
}

void gerarCodigoChamadaFunc(struct Arv *func, FILE *out) { 
    if(func->listaParam) {
        struct Parametro *tmp = func->listaParam;
        while(tmp) {
            gerarCodigoExprAritm(tmp->exprAritm, out);
            tmp = tmp->proximo;
        }

        char *listaTipoParam = gerarListaParam(func->listaParam);
        fprintf(out, "\tinvokestatic TrabalhoCOM/%s(%s)I\n", func->arv1->valorString, listaTipoParam); // VERIFICAR AQUI
        free(listaTipoParam);
    }

    else fprintf(out, "\tinvokestatic TrabalhoCOM/%s(V)I\n", func->arv1->valorString);

    return;
}

void gerarCodigoExprAritm(struct Arv *exprAritm, FILE *out) {
    if(exprAritm->op < 4) {
        gerarCodigoExprAritm(exprAritm->arv1, out);
        gerarCodigoExprAritm(exprAritm->arv2, out);
        switch(exprAritm->op) {
            case ADD:
                fprintf(out, "\tiadd\n");
                break;
            case SUB:
                fprintf(out, "\tisub\n");
                break;
            case MUL:
                fprintf(out, "\timul\n");
                break;
            case DIV:
                fprintf(out, "\tidiv\n");
                break;
        }
    }

    else{
        switch(exprAritm->op) {
            case ID:
                fprintf(out, "\tiload %d\n", exprAritm->local);
                break;
            case FUNC:
                gerarCodigoChamadaFunc(exprAritm, out);
                break;
            case INT:
                gerarCodigoInteiro(exprAritm->valorInt, out);
                break;
        }
    }
    
    return;
}

void gerarCodigoExprRel(struct Arv *exprRel, const char *ltrue, FILE *out) {
    gerarCodigoExprAritm(exprRel->arv1, out);
    gerarCodigoExprAritm(exprRel->arv2, out);
    switch(exprRel->op) {            
        case MAIG:
            fprintf(out, "\tif_icmpge %s\n", ltrue);
            break;
        case MAIOR:
            fprintf(out, "\tif_icmpgt %s\n", ltrue);
            break;
        case MEIG:
            fprintf(out, "\tif_icmple %s\n", ltrue);
            break;
        case MENOR:
            fprintf(out, "\tif_icmplt %s\n", ltrue);
            break;
        case IGUAL:
            fprintf(out, "\tif_icmpeq %s\n", ltrue);
            break;
        case DIF:
            fprintf(out, "\tif_icmpne %s\n", ltrue);
            break;
    }

    return;
}

void gerarCodigoExprLog(struct Arv *exprLog, const char *ltrue, const char *lfalse, FILE *out) {
    switch(exprLog->op) {
        case AND:
            const char *label_segundoTermo = gerarLabel();
            gerarCodigoExprLog(exprLog->arv1, label_segundoTermo, lfalse, out);
            gerarCodigoExprLog(exprLog->arv2, ltrue, lfalse, out);
            break;
        case OR:
            label_segundoTermo = gerarLabel();
            gerarCodigoExprLog(exprLog->arv1, ltrue, label_segundoTermo, out);
            gerarCodigoExprLog(exprLog->arv2, ltrue, lfalse, out);
            break;
        case NEG:
            gerarCodigoExprLog(exprLog->arv1, lfalse, ltrue, out);
            break;
        default:
            gerarCodigoExprRel(exprLog, ltrue, out);
    }

    return;
}

void gerarCodigoIf(struct Arv *cmd, FILE *out) {
    /* gerar 2 labels (bloco true e continuação do programa)
    *  dependendo da expressão lógica pode have mais labels
    *  se houver bloco else, mais 1 label
    */

    const char *ltrue = gerarLabel(), *lfalse = gerarLabel();
    gerarCodigoExprLog(cmd->arv1, ltrue, lfalse, out);
    fprintf(out, "\tgoto %s\n", lfalse);
    fprintf(out, "\t%s:\n", ltrue);
    gerarCodigoBloco(cmd->bloco1, NULL, 0, out);
    
    if(cmd->bloco2) {
        const char *l3 = gerarLabel();
        fprintf(out, "\tgoto %s\n", l3);
        fprintf(out, "\t%s:\n", lfalse);
        gerarCodigoBloco(cmd->bloco2, NULL, 0, out);
        fprintf(out, "\t%s:\n", l3);
    }
    
    else fprintf(out, "%s:\n", lfalse);    

    return;
}

void gerarCodigoWhile(struct Arv *cmd, FILE *out) {
    const char *labelWhile = gerarLabel(), *labelContinuar = gerarLabel(), *labelSair = gerarLabel();
    fprintf(out, "\t%s:\n", labelWhile);
    gerarCodigoExprLog(cmd->arv1, labelContinuar, labelSair, out);
    fprintf(out, "\tgoto %s\n", labelSair);
    fprintf(out, "\t%s:\n", labelContinuar);
    gerarCodigoBloco(cmd->bloco1, NULL, 0, out);
    fprintf(out, "\tgoto %s\n", labelWhile);
    fprintf(out, "\t%s:\n", labelSair);

    return;
}

void gerarCodigoAtrib(struct Arv *cmd, FILE *out) {
    gerarCodigoExprAritm(cmd->arv2, out);
    fprintf(out, "\tistore %d\n", cmd->arv1->local);

    return;
}

void gerarCodigoWrite(struct Arv *cmd, FILE *out) {
    fprintf(out, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
    gerarCodigoExprAritm(cmd->arv1, out);
    fprintf(out, "\tinvokevirtual java/io/PrintStream/println(I)V\n");

    return;
}

void gerarCodigoReturn(struct Arv *cmd, FILE *out) {
    if(cmd->arv1) {
        gerarCodigoExprAritm(cmd->arv1, out);
        fprintf(out, "\tireturn\n");
    }

    else fprintf(out, "\treturn\n");

    return;
}

void gerarCodigoComando(struct Arv *cmd, FILE *out) {
    /* não gera código para leitura e somente para tipo inteiro */
    if(!cmd || !out) return;

    switch(cmd->op) {
        case IF:
            gerarCodigoIf(cmd, out);
            break;
        case WHILE:
            gerarCodigoWhile(cmd, out);
            break;
        case ATRIB:
            gerarCodigoAtrib(cmd, out);
            break;
        case WRITE:
            gerarCodigoWrite(cmd, out);
            break;
        case FUNC:
            gerarCodigoChamadaFunc(cmd, out);
            break;
        case RETURN:
            gerarCodigoReturn(cmd, out);
            break;
    }
    
    return;
}

void gerarCodigoBloco(struct Bloco *bloco, struct Parametro *listaDeclParam, int flagBP, FILE *out) {
    if(!bloco || !out) return;

    /* 
    *  Se alguma variável dentro do bloco possuir mesmo nome que algum parâmetro da função, deve receber mesmo local.
    *  Deve-se percorrer os comandos do bloco e encontrar todos os nós do tipo ID que tenham mesmo 'valorString' que
    *  os parâmetros da função. Similarmente com as variáveis da tabela de símbolos.
    *
    *  Se o local do último parâmetro da função for i, as k variáveis da tabela de símbolos terão locais i + 1, i + 2,
    *  ..., i + k. Caso não houver parâmetros, as variáveis da tabela de símbolos terão locais 0, 1, ..., k.
    * 
    *  No caso da main, haverá um parâmetro só (args) (local 0) e as variáveis locais estarão indexadas a partir de 1.
    */
    
    if(flagBP) {
        fprintf(out, ".method public static main([Ljava/lang/String;)V\n");
        fprintf(out, "\t.limit stack 8\n");
        fprintf(out, "\t.limit locals 8\n\n");
    }
    
    int n = contarParametros(listaDeclParam);
    
    // setando locais das variáveis declaradas (na tabela de símbolos)
    if(bloco->declaracoes) {
        struct TabelaSimb *decl = bloco->declaracoes;

        if(n || flagBP) n++;
        
        while(decl) {                
            decl->local = n;
            n++;
            decl = decl->proximo;
        }
    }
    
    // setando locais das variáveis no bloco (expressões aritméticas) e gerando código para cada comando
    struct Arv* tmp = bloco->listaCmds;
    while(tmp) {
        setLocais(tmp, bloco->declaracoes, listaDeclParam);
        gerarCodigoComando(tmp, out);
        tmp = tmp->proximo;
    }

    if(flagBP) fprintf(out, ".end method\n");

    return;
}

const char *gerarTipo(struct Arv *tipo) {    

    if(!strcmp(tipo->valorString, "int")) return "I";
    else if(!strcmp(tipo->valorString, "float")) return "F";
    else if(!strcmp(tipo->valorString, "void")) return "V";    
}

char *gerarDeclParamFunc(struct Parametro *listaPar) {    
    if(!listaPar) return "";
    
    char *listaTipos = malloc(sizeof(char));

    struct Parametro *tmp = listaPar;
    int local = 0;
    while(tmp) {
        tmp->local = local;
        local++;
        listaTipos = realloc(listaTipos, (local + 1) * sizeof(char));
        strcat(listaTipos, gerarTipo(tmp->tipo));
        tmp = tmp->proximo;
    }

    return listaTipos;
}

void gerarCodigoFuncao(struct Funcao* func, FILE *out) {
    if(!func || !out) return;
    
    if(func->declPar) {
        char *declPar = gerarDeclParamFunc(func->declPar);
        fprintf(out, ".method public static %s(%s)%s\n", func->nome->valorString, declPar, gerarTipo(func->tipoRetorno));
        free(declPar);
    }
    else fprintf(out, ".method public static %s()%s\n", func->nome->valorString, gerarTipo(func->tipoRetorno));
    // depois calcular certinho o stack
    fprintf(out, "\t.limit stack 8\n");
    fprintf(out, "\t.limit locals 8\n\n");

    gerarCodigoBloco(func->blocoPrincipal, func->declPar, 0, out);

    fprintf(out, ".end method\n\n");

    return;
}

void gerarCodigoFuncoes(struct Funcao *func, FILE *out) {
    if(!func || !out) return;

    struct Funcao *tmp = func;
    while(tmp) {
        gerarCodigoFuncao(tmp, out);        
        tmp = tmp->proximo;
    }

    return;
}

void gerarCodigo(struct Programa *prog) {
    if(!prog || erro) return;

    FILE *out = fopen("saida.j", "wt");
    if(!out) {perror("erro ao criar arquivo de saida"); return;}
    
    // header    
    fprintf(out, ".class public TrabalhoCOM\n");
    fprintf(out, ".super java/lang/Object\n\n");

    // construtor
    fprintf(out, ".method public <init>()V\n");
    fprintf(out, "\t.limit stack 1\n");
    fprintf(out, "\t.limit locals 1\n\n");
    fprintf(out, "\taload_0\n");
    fprintf(out, "\tinvokespecial java/lang/Object/<init>()V\n");
    fprintf(out, "\treturn\n");
    fprintf(out, ".end method\n\n");
    
    if(prog->funcoes) gerarCodigoFuncoes(prog->funcoes, out);
    gerarCodigoBloco(prog->blocoPrincipal, NULL, 1, out);
    
    fclose(out);

    return;
}

/**********************************************************************************************************************
******************************************** LIBERAÇÃO DE MEMÓRIA *****************************************************
**********************************************************************************************************************/
void freeBloco(struct Bloco *bloco);

void freeExprAritmRel(struct Arv *arv) {
    if (!arv) return;

    // Libera os nós filhos
    if(arv->arv1) freeExprAritmRel(arv->arv1);
    if(arv->arv2) freeExprAritmRel(arv->arv2);

    // Libera o nó atual
    free(arv);
}

void freeExprLog(struct Arv *arv) {    
    freeExprAritmRel(arv->arv1);
    if(!(arv->op == NEG)) freeExprAritmRel(arv->arv2);

    free(arv);

    return;
}
void freeParametro(struct Parametro *param) {
    if (!param) return;

    // Libere a memória para os campos da estrutura
    if(param->nome) free(param->nome);
    if(param->tipo) free(param->tipo);
    if(param->exprAritm) freeExprAritmRel(param->exprAritm);
    // Libere a memória para a própria estrutura
    free(param);
}

// Função para liberar a memória alocada para uma lista de parâmetros
void freeListaParametros(struct Parametro *listaParam) {
    if(!listaParam) return;
    
    struct Parametro *tmp = listaParam;
    
    while (tmp->proximo) {
        tmp = tmp->proximo;
        freeParametro(tmp->anterior);        
    }

    free(tmp);

    return;
}

void freeComando(struct Arv *cmd) {
    switch(cmd->op) {
        case IF:
            freeExprLog(cmd->arv1);
            freeBloco(cmd->bloco1);
            if(cmd->bloco2) freeBloco(cmd->bloco2);
            break;
        case WHILE:
            freeExprLog(cmd->arv1);
            freeBloco(cmd->bloco1);
            break;
        case ATRIB:
            free(cmd->arv1);
            if(cmd->arv2->op == LITERAL) free(cmd->arv2);
            else freeExprAritmRel(cmd->arv2);
            break;
        case WRITE:
            if(cmd->arv1) freeExprAritmRel(cmd->arv1);
            else free(cmd->arv2);
            break;
        case READ:
            free(cmd->arv1);
            break;
        case FUNC:
            free(cmd->arv1);
            if(cmd->listaParam) freeListaParametros(cmd->listaParam);
            break;
        case RETURN:
            if(cmd->arv1) {
                if(cmd->arv1->op == LITERAL) free(cmd->arv1);
                else freeExprAritmRel(cmd->arv1);
            }            
            break;
    }

    free(cmd);

    return;
}

void freeCmds(struct Arv *listaCmds) {
    struct Arv *tmp = listaCmds;
    
    while(tmp->proximo) {
        tmp = tmp->proximo;
        free(tmp->anterior);
    }
    
    freeComando(tmp);
    
    return;
}

void freeDeclaracoes(struct TabelaSimb *decl) {
    struct TabelaSimb *tmp = decl;
    
    while(tmp->proximo) {
        free(tmp->tipo);
        free(tmp->id);
        // free(tmp->local);
        
        tmp = tmp->proximo;
        free(tmp->anterior);
    }

    free(tmp->tipo);
    free(tmp->id);
    // free(tmp->local);
    free(tmp);

    return;
}

void freeBloco(struct Bloco *bloco) {
    if (!bloco) return;

    if(bloco->declaracoes) freeDeclaracoes(bloco->declaracoes);    
    freeCmds(bloco->listaCmds);

    free(bloco);
}

void freeDeclPar(struct Parametro *par) {
    struct Parametro *tmp = par;
    
    while(tmp->proximo) {
        free(tmp->tipo);
        free(tmp->nome);
        
        tmp = tmp->proximo;
        free(tmp->anterior);
    }

    free(tmp->tipo);
    free(tmp->nome);
    free(tmp);

    return;
}

void freeFuncoes(struct Funcao *funcoes) {
    struct Funcao *tmp = funcoes;
    while(tmp->proximo) {
        free(tmp->tipoRetorno);
        free(tmp->nome);
        if(tmp->declPar) freeDeclPar(tmp->declPar);
        freeBloco(tmp->blocoPrincipal);
        
        tmp = tmp->proximo;
        free(tmp->anterior);
    };

    free(tmp->tipoRetorno);
    free(tmp->nome);
    if(tmp->declPar) freeDeclPar(tmp->declPar);
    freeBloco(tmp->blocoPrincipal);

    free(tmp);

    return;
}

void freePrograma(struct Programa *prog) {
    if (!prog) return;

    if(prog->funcoes) freeFuncoes(prog->funcoes);
    freeBloco(prog->blocoPrincipal);

    free(prog);

    return;
}
