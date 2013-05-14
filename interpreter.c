#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "erro.h"

#define TABELA_TAMANHO_INICIAL 10
#define TABELA_TAMANHO_INC 50

int tamanhoTabela = 0;

int separaStrEmArgumentos(char* str, char*** args ){
    
    // substitui todos os espaços e parágrafos por \0 e dá um apontador para a posição seguinte
    int i=0;
    int nargs = 2;
    for(i=0; str[i]; i++){
        if(str[i] == ' ')
            nargs++;
    }
    
    *args = (char**)malloc( sizeof(char) * nargs);
    
    i=0;
    (*args)[i] = strtok(str, " \n");
    while((*args)[i++] != NULL)
        (*args)[i] = strtok(NULL, " \n");
    
    // verifica os argumentos
    if((*args)[0] == NULL)
        return setError(err_argumentoInvalido, __LINE__, __FILE__);
    
    return err_sucesso;
}

int inicializaTabela(){
    if(tamanhoTabela != 0)
        return 0; // se a tabela já tiver sido alocada, não faz nada
    
    tabela = (Tabela*)malloc(sizeof(Tabela) * TABELA_TAMANHO_INICIAL);
    
    // ERRO MALLOC
    
    int i;
    for(i=0; i<TABELA_TAMANHO_INICIAL; i++){
        tabela[i].chave = NULL;
        tabela[i].valor = NULL;
    }
    tamanhoTabela = TABELA_TAMANHO_INICIAL;
    
    return 0;
}

int aumentaTabela(){
    tamanhoTabela += TABELA_TAMANHO_INC;
    tabela = (Tabela*)realloc(tabela, sizeof(Tabela) * (tamanhoTabela));
    
    int i;
    for(i = tamanhoTabela-TABELA_TAMANHO_INC; i< tamanhoTabela; i++){
        tabela[i].chave = NULL;
        tabela[i].valor = NULL;
    }
        
    
    // ERRO MALLOC
    
    return 0;
}

int insereNaTabela(char *chave, char *valor){
    if(chave == NULL || valor == NULL)
        return err_chaveOuValorNulo;
    
    int i=0;
    for(i=0; i<tamanhoTabela; i++){
        if(tabela[i].chave == NULL)
            break;
    }
    
    if(i == tamanhoTabela){ // tabela está cheia
        aumentaTabela();
    } // o i nao se altera
    
    // i vai ser a posição do primeiro elemento vazio, copiar os valores para lá
    tabela[i].chave = (char*)malloc(sizeof(char) * (strlen(chave)+1));
    strcpy(tabela[i].chave, chave);
    tabela[i].valor = (char*)malloc(sizeof(char) * (strlen(valor)+1));
    strcpy(tabela[i].valor, valor);
    
    return 0;
}

int agrupaChavesTabela(){
    
}

int printTabela(){
    int i;
    for(i=0; i<tamanhoTabela; i++){
        if(tabela[i].chave != NULL)
            printf("%s:%s\n", tabela[i].chave, tabela[i].valor);
        else
            break;
    }
}
