#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comum.h"
#include "erro.h"

void *myMalloc(size_t size){
    void *ptr = malloc(size);
    if( ptr == NULL )
        printErrorAndExit("Falha ao alocar memória.", __FILE__, __LINE__);
    return ptr;
}

void *myRealloc(void *ptr, size_t size){
    ptr = realloc(ptr, size);
    if( ptr == NULL )
        printErrorAndExit("Falha ao realocar memória.", __FILE__, __LINE__);
    return ptr;
}

void filhoMorreu(){
    zombieFound = true;
}

void separaStrEmArgumentos(char* str, char*** args ){
    
    // substitui todos os espaços e parágrafos por \0 e dá um apontador para a posição seguinte
    int i=0;
    int nargs = 2;
    for(i=0; str[i]; i++){
        if(str[i] == ' ')
            nargs++;
    }
    
    *args = (char**)myMalloc( sizeof(char) * nargs);
    
    i=0;
    (*args)[i] = strtok(str, " \n");
    while((*args)[i++] != NULL)
        (*args)[i] = strtok(NULL, " \n");
    
    // verifica os argumentos
    if((*args)[0] == NULL)
        printErrorAndExit("Argumento inválido.", __FILE__, __LINE__);
}


