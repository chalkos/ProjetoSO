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
#include <string.h>

// locais
#include "map.h"
#include "erro.h"
#include "interpreter.h"
#include "bool.h"

#define stdInBufferSize 65536 //64 Kb
#define argBufferSize 4096

char stdInBuffer[stdInBufferSize];
int stdInBufferPos = stdInBufferSize;
int stdInBufferLidos = 0;

int newStdin;

bool abortMap = false; // fica true caso tenha de se abortar o comando map

//pipes para receber dados do stdout dos comandos nas posições 0 e 1
//a posição 2 é o pid do filho
//a posição 3 é um booleano que indica se o processo ainda existe
pipes pOut[MAXCOM];

int esperaPorPipe(){
    int resultado;
    int pid = wait(&resultado);
    
    if(pid == -1)
        return -1;
    
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
    
    if(!encontrou)
        return -1;
    return i;
}

int map(char *comando){
    char **args;
    if( separaStrEmArgumentos(comando, &args) != 0 ){
        return -1;
    }
    int i=0;
    for(i=0; args[i]; i++);
    
    args = (char**)realloc(args, sizeof(char) * (i+2)); //acrescentar uma posição extra ao array (para a linha argumento)
    args[i+1] = NULL; //meter o NULL nessa posição
    
    int indiceArgsBuffer = i;
    
    // utilizar o ficheiro de stdinDebug
    int stdinDebugFD = open("stdinDebug", O_RDONLY); //abrir o ficheiro
    dup2(stdinDebugFD, 0); //o stdin passa a ser esse ficheiro
    
    //criar uma cópia do stdin, para poder ler os dados do stdin 
    //à medida que vai sendo preciso (lazy)
    //newStdin = dup(0);
    
    i=0;
    bool livre = true;
    
    char buffer[argBufferSize];
    
    for(i=0; i<MAXCOM; i++){
        pOut[i].pid = -1;
        pOut[i].vivo = false;
    }
    i=0;
    
    inicializaTabela();
    
    
    while(1){
        if(lerLinha(buffer, argBufferSize) == -1)
            break;
        
        if(!livre){
            i = esperaPorPipe();
            if( i == -1 )
                printf("BAAM!\n");
        }
        
        
        if(pOut[i].vivo == false && pOut[i].pid > 0){
            lerChaveValor(i);
            close(pOut[i].pipe[0]); //fechar o que resta do pipe
            pOut[i].pid = -1;
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
            
            args[indiceArgsBuffer] = buffer;
            
            execvp(args[0], args);
            
            
            //printf("%s %s %s\n", args[0], args[1], args[2]);
            
            
            //sleep(1);
            
            //exit(EXIT_SUCCESS);
            exit(EXIT_FAILURE); //o comando deve terminar naturalmente e nao chegar aqui
        }
        
        //printf("criado pid %d\n", pOut[i].pid);
        
        // fechar a entrada do pipe no pai
        close(pOut[i].pipe[1]);
        
        livre = false;
        for(i=0; i<MAXCOM; i++){
            if(pOut[i].vivo == false){
                livre = true;
                break;
            }
        }
    }
    
    while((i = esperaPorPipe()) != -1){
        lerChaveValor(i);
        close(pOut[i].pipe[0]); //fechar o que resta do pipe
        pOut[i].pid = -1; // sinalizar o pipe como terminado
    }
    
    printTabela();
}

int lerChaveValor(int pos){
    const int tamanhoInicial = 50;
    int tamanho = tamanhoInicial;
    int incrementos = 1;
    
    char *buffer = (char*)malloc(sizeof(char) * tamanho);
    char *i = buffer;
    int lidos;
    
    while((lidos = read(pOut[pos].pipe[0], i, sizeof(char)*tamanhoInicial)) > 0){
        if( lidos < tamanhoInicial){
            //meter \0 no fim
            *(i+lidos-1) = '\0'; //>>>>>>>>>>>>>>>>>>>>>>>>cuidado com o ultimo valor do stdin
            break; // não vai ler mais nada, evita fazer mais um read
        }else{
            //lidos == tamanho-1, quer dizer que há mais para ler
            incrementos++;
            buffer = (char*)realloc(buffer, sizeof(char) * tamanhoInicial * incrementos);
            
            i = buffer + tamanhoInicial * (incrementos-1);
        }
    }
    
    char *chave, *valor;
    
    chave = strtok(buffer, "\t");
    valor = strtok(NULL, "\n");
    while( chave != NULL && valor != NULL ){
        insereNaTabela(chave, valor);
        chave = strtok(NULL, "\t");
        valor = strtok(NULL, "\n");
    }
    
    
    
    
    
    free(buffer);
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
            buffer[lidosParaBuffer] = '\0';
            break;
        }
    }
    
    return 0;
}