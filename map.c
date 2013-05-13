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

#define stdInBufferSize 65536 //64 Kb

char stdInBuffer[stdInBufferSize];
int stdInBufferPos = stdInBufferSize;
int stdInBufferLidos = 0;

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
    //newStdin = dup(0);
    
    int i=0; //indice do pipe
    
    char buffer[4096];
    
    int pOut[MAXCOM][2]; //pipes para receber dados do stdout dos comandos
    bool todosPipesUsados = false;
    
    
    while(1){
        if(lerLinha(buffer, 4096) != -1)
            break;
        
        if(todosPipesUsados){
            setPipeLivre(&i);
        }
            
        
        pipe(pOut[i]);
        
        
        // fazendo forks
        if(fork()==0){
            // fechar a saida do pipe no filho
            close(pOut[i][0]);
            // o stdout do filho passa a ser a entrada do pipe
            dup2(pOut[i][1], 1);
            // execvp
            
            
            exit(EXIT_FAILURE); //o comando deve terminar naturalmente e nao chegar aqui
        }
        
        // fechar a entrada do pipe no pai
        close(pOut[i][1]);
        
        // limitar para ter apenas alguns ficheiros em execucao (variavel "emExecucao")
        // arranjar maneira de receber a informação de volta (ter 1 pipe para cada)
        
        if(!todosPipesUsados && i==MAXCOM-1)
            todosPipesUsados = true;
    }
    
}

int setPipeLivre(int &i){
    // vai fazer o wait, quando receber um wait, vai ver qual é o nr que ele retorna e mete isso no i
    // também verifica se o processo filho terminou com erro
    int resultado;
    wait(&resultado);
    
    if(WIFEXITED(resultado)){
        resultado = WEXITSTATUS(resultado);
        // terminou 
        if(resultado >= 0 && resultado < MAXCOM)
            *i = resultado;
    }else{
        // cópia terminou com erro
    }
}

int lerLinha(char *buffer, int bufferSize){
    if(stdInBufferPos == -1)
        return -1;
    
    int lidosParaBuffer = 0;
    bool sair = false; //sai quando encontra um \n
    
    while(!sair){
        if(stdInBufferPos >= stdInBufferSize){
            // se o stdin estiver vazio apenas meter pos=-1 e sair
            if(( stdInBufferLidos = read(0, stdInBuffer, stdInBufferSize)) <= 0){
                // se chegar aqui, significa que escreveu qualquer coisa para o buffer
                // logo temos de terminar o buffer
                buffer[lidosParaBuffer-1] = '\0';
                
                stdInBufferPos = -1;
                break;
            }
            stdInBufferPos = 0;
        }
        
        /*
         * Copiar enquanto houver espaço no buffer
         *      e enquanto a posição no stdinBuffer for menor que o tamanho do stdInBuffer
         *      e enquanto a posição no stdinBuffer for menor que o nr de caracteres lidos do stdin
         */
        while( !sair && lidosParaBuffer < bufferSize && stdInBufferPos < stdInBufferSize && stdInBufferPos < stdInBufferLidos){
            if(stdInBuffer[stdInBufferPos] == '\n'){
                buffer[lidosParaBuffer++] = '\0';
                sair = true;
            }else
                buffer[lidosParaBuffer++] = stdInBuffer[stdInBufferPos];
            
            stdInBufferPos++;
        }
        
        
        
        // se buffer estiver cheio, mete um \0 na ultima posicao e percorre o resto do stdInBuffer
        if(lidosParaBuffer >= bufferSize){
            buffer[bufferSize-1] = '\0';
            
            // acabar de ler o buffer até ao \n, ou até ler tudo o que tinha a ler, ou quando ler o buffer todo
            if(stdInBuffer[stdInBufferPos-1] == '\n')
                sair = true;
            
            while(!sair && stdInBuffer[stdInBufferPos++] != '\n' && stdInBufferPos < stdInBufferSize && stdInBufferPos < stdInBufferLidos)
                ;
            
            if(stdInBuffer[stdInBufferPos-1] == '\n')
                sair = true;
        }
        
        // se acabou de ler o que restava do stdin
        if(stdInBufferPos >= stdInBufferLidos && stdInBufferLidos < stdInBufferSize){
            stdInBufferPos = -1;
            break;
        }
    }
    
    return 0;
}