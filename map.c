//open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// write
#include <unistd.h>

// wait
#include <sys/types.h>
#include <sys/wait.h>

// kill
#include <sys/types.h>
#include <signal.h>

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

bool abortMap = false; // fica true caso tenha de se abortar o comando map

//pipes para receber dados do stdout dos comandos nas posições 0 e 1
//a posição 2 é o pid do filho
//a posição 3 é um booleano que indica se o processo ainda existe
pipes pOut[MAXCOM];

void setPipeFree(){
    int resultado;
    int pid = wait(&resultado);
    
    //printf("MY PID IS: %d\n", getpid());
    
    if(!WIFEXITED(resultado)){
        // terminou abruptamente
        abortMap = true;
    }
    
    int i;
    bool encontrou = false;
    for(i=0; i<MAXCOM; i++){
        //printf("-----[%d] %d; %d; %d; %d\n", i, pOut[i].pipe[0], pOut[i].pipe[1], pOut[i].pid, pOut[i].vivo);
        if(pOut[i].pid == pid){
            encontrou = true;
            pOut[i].vivo = false;
            break;
        }
    }
    
    return;
    
    if(encontrou)
        printf("encontrou na pos %d!\n", i);
    else
        printf("nao encontrou o pid %d\n", pid);
}

int map(char *comando){
    char** args;
    /*if( separaStrEmArgumentos(comando, args) != 0 ){
        return -1;
    }*/
    
    signal(SIGCHLD, setPipeFree);
    
    // utilizar o ficheiro de stdinDebug
    int stdinDebugFD = open("stdinDebug", O_RDONLY); //abrir o ficheiro
    dup2(stdinDebugFD, 0); //o stdin passa a ser esse ficheiro
    
    //criar uma cópia do stdin, para poder ler os dados do stdin 
    //à medida que vai sendo preciso (lazy)
    //newStdin = dup(0);
    
    int i=0; //indice do pipe
    
    char buffer[4096];
    
    for(i=0; i<MAXCOM; i++)
        pOut[i].vivo = false;
    i=0;
    bool todosPipesUsados = false;
    
    
    
    
    while(1){
        if(lerLinha(buffer, 4096) == -1)
            break;
        
        if(todosPipesUsados){
            if(setPipeLivre(&i) == -1){
                // houve um erro na execução de um comando
                //terminar os outros
            }
            char c;
            while(read(pOut[i].pipe[0], &c, sizeof(char)) > 0){
                printf("%c", c);
            }
            
            close(pOut[i].pipe[0]); //fechar o que resta do pipe
        }
            
        // criar um pipe na posição i
        pipe(pOut[i].pipe);
        pOut[i].vivo = true;
        
        
        
        // fazendo forks
        if( (pOut[i].pid = fork())==0){
            // fechar a saida do pipe no filho
            close(pOut[i].pipe[0]);
            // o stdout do filho passa a ser a entrada do pipe
            dup2(pOut[i].pipe[1], 1);
            // execvp
            
            printf("filho #%d - %s\n", i, buffer);
            
            exit(EXIT_SUCCESS);
            //exit(EXIT_FAILURE); //o comando deve terminar naturalmente e nao chegar aqui
        }
        
        //printf("criado pid %d\n", pOut[i][2]);
        
        // fechar a entrada do pipe no pai
        close(pOut[i].pipe[1]);
        
        // limitar para ter apenas alguns ficheiros em execucao (variavel "emExecucao")
        // arranjar maneira de receber a informação de volta (ter 1 pipe para cada)
        
        
        if(!todosPipesUsados && i==MAXCOM-1)
            todosPipesUsados = true;
        
        if(!todosPipesUsados)
            i++;
    }
    
    
    
}

int setPipeLivre(int *livre){
    // vai enviar um sinal «zero» aos processos filhos
    // se houver erro, o processo já não existe, e pode-se renovar o pipe
    // se funcionar, o processo ainda está a correr
    // também verifica se o processo terminou de forma inesperada
    int i;
    
    while(1){
        for(i=0; i<MAXCOM; i++){
            if(pOut[i].vivo == false){
                *livre = i;
                return 0;
            }
        }
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