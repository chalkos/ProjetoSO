//malloc, realloc
#include <stdlib.h>

//strcmp
#include <string.h>

//printf
#include <stdio.h>

#include "lista.h"
#include "bool.h"
#include "erro.h"

const int tamanhoInicial = 10; //nr de elementos do array
const int tamanhoIncremento = 10; //nr de elementos a adicionar ao array a cada redimensionamento
int nListas = 0;

int listaPush( int indice, char *valor ){
    // alocar o novo elemento
    Elem *elem = (Elem*)malloc(sizeof(Elem));
    
    // copiar a string
    elem->valor = (char*)malloc(sizeof(char) * (strlen(valor)+1));
    strcpy(elem->valor, valor);
    
    // colocar à cabeça da lista
    elem->proximo = listas[indice].valores;
    listas[indice].valores = elem;
    
    // actualiza o numero de elementos
    listas[indice].nElems++;
}

char *listaPop( int indice ){
    Elem *elem = listas[indice].valores;
    
    if(elem == NULL) //lista vazia
        return NULL;
    
    char *valor = elem->valor;
    
    listas[indice].valores = elem->proximo;
    
    free(elem);
    
    // actualiza o numero de elementos
    listas[indice].nElems--;
    
    return valor;
}

int listaInsere(char *chave, char *valor){
    int i;
    
    // verificar se a chave está no array
    for( i=0; i<nListas; i++ ){
        if( listas[i].chave != NULL && strcmp(listas[i].chave, chave) == 0 ){
            if( listaPush(i, valor) == 0 ){
                // adicionou o valor com sucesso
                return 0;
            }else
                return err_adicionarValor;
        }
    }
    
    // se chegar aqui, a chave não está no array
    for( i=0; i<nListas; i++ ){
        if( listas[i].chave == NULL ){
            
            listas[i].chave = (char*)malloc(sizeof(char) * (strlen(chave)+1));
            strcpy(listas[i].chave, chave);
            
            if( listaPush(i, valor) == 0 ){
                // adicionou o valor com sucesso
                return 0;
            }else
                return err_adicionarValor;
        }
    }
    
    // se chegar aqui, o array não tem elementos livres.
    // aumentar e inserir na primeira das novas posicoes, evitando percorrer outra vez o array
    listaAumenta();
    i = nListas - tamanhoIncremento;
    listas[i].chave = (char*)malloc(sizeof(char) * (strlen(chave)+1));
    strcpy(listas[i].chave, chave);

    if( listaPush(i, valor) == 0 ){
        // adicionou o valor com sucesso
        return 0;
    }else
        return err_adicionarValor;
    
}

void listaAumenta(){
    nListas += tamanhoIncremento; //aumenta 10 elementos de cada vez
    listas = (Lista*)realloc(listas, sizeof(Lista) * nListas);
    
    int i;
    for( i= nListas-tamanhoIncremento; i<nListas; i++){
        listas[i].chave = NULL;
        listas[i].nElems = 0;
        listas[i].valores = NULL;
    }
}

void listaInit(){
    listas = (Lista*)malloc(sizeof(Lista) * tamanhoInicial);
    
    int i;
    for( i=0; i<tamanhoInicial; i++){
        listas[i].chave = NULL;
        listas[i].nElems = 0;
        listas[i].valores = NULL;
    }
    nListas = tamanhoInicial;
}

void listaPrint(){
    Elem *elem;
    
    int i;
    for(i=0; i<nListas; i++){
        printf("[%d] chave: %s (#elems: %d)\n", i, listas[i].chave, listas[i].nElems);
        elem = listas[i].valores;
        while( elem != NULL ){
            printf("    valor: %s\n", elem->valor);
            elem = elem->proximo;
        }
    }
}

void listaReduced(int indice, char *reducedValue){
    // copiar a string
    listas[indice].reduced = (char*)malloc(sizeof(char) * (strlen(reducedValue)+1));
    strcpy(listas[indice].reduced, reducedValue);
}