#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int size_X;
int size_Y;
char ** labyrinthe=NULL;

int main() {
	printf("Entrez la taille du labyrinthe: ");
	scanf("%d%d", &size_X, &size_Y);
	printf("Le labyrinthe est de taille %dx%d\n", size_X, size_Y);
	return  0;
}