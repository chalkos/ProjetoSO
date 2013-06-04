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
#include "bool.h"
#include "comum.h"
#include "lista.h"

#define stdInBufferSize 65536 //64 Kb
#define argBufferSize 4096

char stdInBuffer[stdInBufferSize];
int stdInBufferPos = stdInBufferSize;
int stdInBufferLidos = 0;

int esperaPorPipeMap(){
    // se não houver processos zombies, ir esvaziando pipes
    // evitando assim um deadlock quando um filho enche o pipe e o pai fica a
    // espera que ele morra para poder ler a informação e o filho fica a espera
    // que o pai lhe esvazie o pipe
    int i=0;
    for(i=0; i<MAXCOM && !zombieFound; i++){
        lerChaveValor(i);
    }
    
    int resultado;
    int pid = wait(&resultado);
    zombieFound = false;
    
    if(pid == -1)
        return -1; // o processo não tem mais filhos a que possa fazer wait
        
    
    if(!WIFEXITED(resultado)){
        // terminou abruptamente
        printErrorAndExit("Uma execução do comando map terminou abruptamente.", __FILE__, __LINE__);
    }
    
    bool encontrou = false;
    for(i=0; i<MAXCOM; i++){
        if(pOut[i].pid == pid){
            encontrou = true;
            pOut[i].vivo = false;
            break;
        }
    }
    
    if(!encontrou)
        printErrorAndExit("O pid do zombie encontrado não estava na lista.", __FILE__, __LINE__);
    
    return i;
}

int map(char *comando){
    char **args;
    separaStrEmArgumentos(comando, &args);
    
    int i=0;
    for(i=0; args[i]; i++);
    
    args = (char**)myRealloc(args, sizeof(char) * (i+2)); //acrescentar uma posição extra ao array (para a linha argumento)
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
    
    listaInit();
    
    while(1){
        if(lerLinha(buffer, argBufferSize) == -1)
            break;
        
        if(!livre){
            i = esperaPorPipeMap();
            if(i == -1)
                printErrorAndExit("Ocorreu um ECHILD n wait antes do tempo.", __FILE__, __LINE__);
        }
        
        
        if(pOut[i].vivo == false && pOut[i].pid > 0){
            lerChaveValor(i);
            close(pOut[i].pipeOut[0]); //fechar o que resta do pipe
            pOut[i].pid = -1;
        }
          
        // criar um pipe na posição i
        if( pipe(pOut[i].pipeOut) == -1 ){
            if( errno == EMFILE)
               printWarning("Too many file descriptors in use by the process. Retrying in a while.", __FILE__, __LINE__);
            else if( errno == EMFILE)
               printWarning("The system limit on the total number of open files has been reached. Retrying in a while.", __FILE__, __LINE__);
            
            sleep(1000);
        }
        pOut[i].vivo = true;
        
        
        // fazendo forks
        while( (pOut[i].pid = fork()) == -1 ){
            if( errno == EAGAIN )
                printWarning("Não foi possível alocar memória para o filho ou o máximo de filhos permitido foi atingido. Tentando outra vez.", __FILE__, __LINE__);
            if( errno == ENOMEM)
                printWarning("failed to allocate the necessary kernel structures because memory is tight. Retrying in a while.", __FILE__, __LINE__);
            
            sleep(1000);
        }
        if( pOut[i].pid==0){
            // fechar a saida do pipe no filho
            close(pOut[i].pipeOut[0]);
            // o stdout do filho passa a ser a entrada do pipe
            dup2(pOut[i].pipeOut[1], 1);
            // execvp
            
            args[indiceArgsBuffer] = buffer;
            
            execvp(args[0], args);
            
            exit(EXIT_FAILURE); //o comando deve terminar naturalmente e nao chegar aqui
        }
        
        //printf("criado pid %d\n", pOut[i].pid);
        
        // fechar a entrada do pipe no pai
        close(pOut[i].pipeOut[1]);
        
        livre = false;
        for(i=0; i<MAXCOM; i++){
            if(pOut[i].vivo == false){
                livre = true;
                break;
            }
        }
    }
    
    while((i = esperaPorPipeMap()) != -1){
        lerChaveValor(i);
        close(pOut[i].pipeOut[0]); //fechar o que resta do pipe
        pOut[i].pid = -1; // sinalizar o pipe como terminado
    }
    
    //listaPrint();
    
}

int lerChaveValor(int pos){
    const int tamanhoInicial = 50;
    int tamanho = tamanhoInicial;
    int incrementos = 1;
    
    char *buffer = (char*)myMalloc(sizeof(char) * tamanho);
    char *i = buffer;
    int lidos;
    int lidosTotal = 0;
    
    while(lidos = read(pOut[pos].pipeOut[0], i, sizeof(char)*tamanhoInicial)){
        if(lidos < 0)
            lidos = 0; // para o lidosTotal não decrescer
        lidosTotal += lidos;
        if( lidos <= 0){
            //meter \0 no fim
            *(i+lidos-1) = '\0';
            break;
        }else{
            //lidos == tamanho-1, quer dizer que há mais para ler
            incrementos++;
            buffer = (char*)myRealloc(buffer, sizeof(char) * tamanhoInicial * incrementos);
            
            i = buffer + lidosTotal;
        }
    }
    
    if(lidosTotal == 0) return -1;
    
    char *chave, *valor;
    
    chave = strtok(buffer, "\t");
    valor = strtok(NULL, "\n");
    while( chave != NULL && valor != NULL ){
        //insereNaTabela(chave, valor);
        listaInsere(chave, valor);
        chave = strtok(NULL, "\t");
        valor = strtok(NULL, "\n");
    }
    
    
    
    
    
    free(buffer);
    return 0;
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