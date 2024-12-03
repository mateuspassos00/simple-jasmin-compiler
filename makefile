all: main

expr.tab.c expr.tab.h: trabalho_comp.y
	bison -d trabalho_comp.y

lex.yy.c: trabalho_comp.l
	flex trabalho_comp.l

main: lex.yy.c trabalho_comp.tab.c
	gcc -o final arv.c lex.yy.c trabalho_comp.tab.c main.c
	
clean: 
	rm -f lex.yy.c trabalho_comp.tab.c trabalho_comp.tab.h final saida.j TrabalhoCOM.class
	
