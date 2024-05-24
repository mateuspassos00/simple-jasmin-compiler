#include <stdio.h>
#include <stdlib.h>
#include "arv.h"

struct Arv* gerarNo(int op, struct Arv* esq, struct Arv* dir) {
    struct Arv* novoNo = (struct Arv*) malloc(sizeof(struct Arv));
    if (!novoNo) perror("Falha ao alocar no");

    novoNo->op = op;
    novoNo->p1 = esq;
    novoNo->p2 = dir;

    return novoNo;
}

struct Arv* gerarFolha(double valor) {
    struct Arv* novaFolha = (struct Arv*) malloc(sizeof(struct Arv));
    if (!novaFolha) perror("Falha ao alocar folha");

    novaFolha->valor = valor;

    return novaFolha;
}

void printArv(struct Arv* arv) {
    if(!(arv->p1 && arv->p2)) { // verificando se o nó é folha
        printf("%.2lf ", arv->valor);
        return;
    }
    printArv(arv->p1); // vai para o nó da esquerda
    switch(arv->op) { // printa operação
        case 0: printf("+ "); break;
        case 1: printf("- "); break;
        case 2: printf("* "); break;
        case 3: printf("/ "); break;
     }
    printArv(arv->p2); // vai para o nó da direita
    return;
}

void freeArv(struct Arv* arv) {
    if(!(arv->p1 && arv->p2)) { // libera nó folha
        free(arv);
        return;
    }
    freeArv(arv->p1); // vai para o nó da esquerda
    freeArv(arv->p2); // vai para o nó da direita
    free(arv); // libera nó
    return;
}
