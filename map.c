//open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// write
#include <unistd.h>

// wait
#include <sys/types.h>
#include <sys/wait.h>

// sprintf
#include <stdio.h>

// erros
#include <errno.h>

// exit
#include <stdlib.h>

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
    
    // utilizar o ficheiro de stdinDebug
    int stdinDebugFD = open("stdinDebug", O_RDONLY); //abrir o ficheiro
    dup2(stdinDebugFD, 0); //o stdin passa a ser esse ficheiro
    
    //criar uma cópia do stdin, para poder ler os dados do stdin 
    //à medida que vai sendo preciso (lazy)
    newStdin = dup(0);
    
    int emExecucao; //numero de ficheiros abertos
    
    int i=1, dummy;
    while(1){
        if( criarFicheiro(i++, &dummy) == -1 )
            break;
        // ir criando ficheiros
        // ir redireccionando fd
        // fazendo forks
        // execvp
        
        // fechar o FD no pai
        close(dummy);
        
        // limitar para ter apenas alguns ficheiros em execucao (variavel "emExecucao")
        // arranjar maneira de receber a informação de volta (ter 1 pipe para cada)
    }
    
}

int criarFicheiro(int nrFicheiro, int *fdResultante){
    if(stdInBufferPos==-1){
        return -1;
    }
        
    
    int i;
    char nomeFicheiro[30];
    sprintf(nomeFicheiro, "tmp_map%d", nrFicheiro);
    
    // abrir um ficheiro para RW, se não existir é criado com permissoes 0660
    // se existir é apagado (O_TRUNC)
    if( (*fdResultante = open(nomeFicheiro, O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) ) == -1){
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

int apagaFicheiros(int ultimoNr){
    return 0;
    
    // precisa de modificações.
    // deverá apagar os ficheiros 1 a 1
    // os nomes devem ser:
    // tmp_map1
    // tmp_map2
    // ...
    // tmp_map<ultimoNr>
    
    // isto apenas deverá ser feito numa segunda fase, em que o map esteja 
    // funcional e mais que testado
    
    if(fork() == 0){
        // apenas o filho vai fazer isto
        char *args[] = {"rm", "-v", "./tmp_map*", NULL};
        //char *args[] = {"pwd",  NULL};
        
        execvp(args[0], args);
        perror("ERROR");

        exit(1); // deverá ser o rm a terminar, e não chegar aqui
    }else{
        // apenas o pai vai fazer isto
        int res;
        wait(&res);
        if(WIFEXITED(res)){
            res = WEXITSTATUS(res);

            printf("RM terminou. Código: %d\n", res);
        }else{
            printf("RM terminou com erro. Código: %d\n", res);
        }
    }
}

