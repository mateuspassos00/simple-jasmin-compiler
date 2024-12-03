#include <stdio.h>
#include "trabalho_comp.tab.h"

extern FILE *yyin;

int main(int argc, char *argv[]) {
	yyin = fopen(argv[1], "rt");
	if(!yyin) {printf("falha ao abrir arquivo\n"); return 1;}
	yyparse();
	fclose(yyin);
	return 0;
}

