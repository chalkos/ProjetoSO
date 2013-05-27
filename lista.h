#ifndef LISTA_H
#define	LISTA_H


typedef struct sElem{
    char *valor;
    struct sElem *proximo;
} Elem;

typedef struct sLista{
    char *chave;
    Elem *valores;
    char *reduced; //valor dado pelo reduce
    int nElems;
} Lista;

Lista *listas;


//strcmp
#include <string.h>

#include "lista.h"
#include "bool.h"
#include "erro.h"

int listaPush( int indice, char *valor );

char *listaPop( int indice );

int listaInsere( char *chave, char *valor);

void listaAumenta();

void listaInit();

void listaPrint();

void listaReduced(char *reducedValue);

#endif	/* LISTA_H */

