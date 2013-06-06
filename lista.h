#pragma once
//strcmp
#include <string.h>

#include "lista.h"
#include "bool.h"
#include "erro.h"

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

void listaPush( int indice, char *valor );

char *listaPop( int indice );

void listaInsere( char *chave, char *valor);

void listaAumenta();

void listaInit();

void listaPrintKeyValues();

void listaPrintKeyReduced();

void listaDumpValues(int indice, int fd);

void listaReduced(int indice, char *reducedValue);

int listaCount();

