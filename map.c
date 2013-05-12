//open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// write
#include <unistd.h>

// sprintf
#include <stdio.h>

// erros
#include <errno.h>

// locais
#include "map.h"
#include "erro.h"
#include "interpreter.h"
#include "bool.h"

#define stdInBufferSize 200

char stdInBuffer[stdInBufferSize];
int stdInBufferPos = stdInBufferSize;

int newStdin;

int map(char *comando){
    char** args;
    /*if( separaStrEmArgumentos(comando, args) != 0 ){
        return -1;
    }*/
    
    //criar uma cópia do stdin, para poder ler os dados do stdin 
    //à medida que vai sendo preciso (lazy)
    newStdin = dup(0); 
    
    int emExecucao;
    
    while(1){
        // ir criando ficheiros
        // ir redireccionando fd
        // fazendo forks
        // execvp
        
        // limitar para ter apenas alguns ficheiros em execucao
        // arranjar maneira de receber a informação de volta (ter 1 pipe para cada)
    }
    
}

int criarFicheiro(int nrFicheiro, int *fdResultante){
    if(stdInBufferPos==-1){
        return -1;
    }
        
    
    int i;
    char nomeFicheiro[30];
    sprintf(nomeFicheiro, "map%d", nrFicheiro);
    
    // abrir um ficheiro para RW, se não existir é criado com permissoes 0660
    if( (*fdResultante = open(nomeFicheiro, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) ) == -1){
        perror( "ouch: " );
    }
    
    
    while(1){
    
        // ler do stdin
        if(stdInBufferPos>=stdInBufferSize){
            
            // se o stdin estiver vazio tambem nao precisa de escrever, apenas meter pos=-1 e fechar o ficheiro
            if(read(newStdin, stdInBuffer, stdInBufferSize) <= 0){
                stdInBufferPos = -1;
                close(*fdResultante);
                break;
            }
            stdInBufferPos = 0;
        }

        // verficar onde está o \n
        for(i=stdInBufferPos; i<stdInBufferSize-1; i++)
            if(stdInBuffer[i] == '\n' || stdInBuffer[i] == '\0'){
                break;
            }
        
        i++;

        // escrever tudo desde stdinbufferPos até e inclusive a posição i
        write(*fdResultante, (char*)(stdInBuffer+stdInBufferPos), i-stdInBufferPos );
        
        if(stdInBuffer[i] == '\0')
            stdInBufferPos = -1; // parar de criar ficheiros porque o stdin está vazio
        else
            stdInBufferPos = i;
        
        if(stdInBufferPos==-1 || i < stdInBufferSize)
            break;
    }
    
}