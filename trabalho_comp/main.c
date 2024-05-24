#include <stdio.h>
#include "trabalho_comp.tab.h"

extern FILE *yyin;

int main()
{
	yyin = fopen("exemplo_in", "rt");
	if(!yyin) {printf("falha ao abrir arquivo\n"); return 1;}
	yyparse();
	fclose(yyin);
	return 0;
}

