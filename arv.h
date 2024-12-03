#ifndef ARV_H
#define ARV_H

enum op {ADD, SUB, MUL, DIV, MAIOR, MAIG, MENOR, MEIG, IGUAL, DIF, AND, OR, NEG, ATRIB,
        WHILE, IF, FUNC, RETURN, READ, WRITE, INT, FLOAT, LITERAL, ID, TIPO,STRING};

// tipo da yylval
struct Atrib{
    struct Programa *prog;
    struct Funcao *func;
    struct Arv *arv;
    struct Parametro *param;
    struct TabelaSimb *tabelaSimb;
    struct Bloco *bloco;
    int valorInt;
    double valorDouble;
    char valorString[50];
};

struct Programa {
    struct Funcao *funcoes;
    struct Bloco *blocoPrincipal;
};

struct Funcao {
    struct Arv *nome, *tipoRetorno;
    struct Parametro *declPar;    
    struct Bloco *blocoPrincipal;
    struct Funcao *anterior, *proximo;
};

struct Parametro {
    // serve tanto para declaração de parâmetros na definição da função quanto para lista de parâmetros na chamada de função
    struct Arv *nome, *tipo, *exprAritm;
    int local;
    struct Parametro *anterior, *proximo;
};

struct TabelaSimb {    
    struct Arv *id, *tipo;
    int local;
    struct TabelaSimb *anterior, *proximo;
};

struct Bloco {
    // caso for um bloco principal poderá haver declarações
    struct TabelaSimb *declaracoes;
    struct Arv *listaCmds;
};

struct Arv { 
    //  nó de propósito geral.. serve para nós folha (int, float, literais, IDs, ...),
    //  nós de operações binárias, unárias e chamadas de função
    enum op op;
    char valorString[20];
    double valorDouble;
    int valorInt;
    int local;
    int parenteses;    
    struct Arv *arv1, *arv2, *anterior, *proximo;
    struct Parametro *listaParam;
    struct Bloco *bloco1, *bloco2;
};

struct Programa *criarPrograma(struct Funcao*, struct Bloco*);
struct Funcao *criarFuncao(struct Arv*, struct Arv*, struct Parametro*, struct Bloco*);
struct Parametro *criarParametro(struct Arv*, struct Arv*, struct Arv*);
struct TabelaSimb *criarTabSimb(struct Arv*, struct Arv*);
struct Bloco *criarBloco(struct TabelaSimb*, struct Arv*);
struct Arv *criarWhile(enum op, struct Arv*, struct Bloco*);
struct Arv *criarIfElse(enum op, struct Arv*, struct Bloco*, struct Bloco*);
struct Arv *criarOpBin(enum op, struct Arv*, struct Arv*);
struct Arv *criarChamadaFuncao(enum op, struct Arv*, struct Parametro*);
struct Arv *criarEscrita(enum op, struct Arv*, struct Arv*);
struct Arv *criarNoFolha(enum op, const char*, int, double);
void printCodigo(struct Programa*);
struct Programa *verificarSemantica(struct Programa *);
void gerarCodigo(struct Programa*);
void freePrograma(struct Programa*);

#endif // ARV_H
