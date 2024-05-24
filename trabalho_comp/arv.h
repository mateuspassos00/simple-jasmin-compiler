enum {ADD, SUB, MUL, DIV};

struct Atrib{
    struct Arv* ptr;
    double valor;
};

struct Arv {
    int op;
    double valor;
    struct Arv *p1, *p2;
};

struct Arv* gerarNo(int, struct Arv*, struct Arv*);
struct Arv* gerarFolha(double);
void freeArv(struct Arv*);
void printArv(struct Arv*);
