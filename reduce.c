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
#include "comum.h"
#include "bool.h"
#include "lista.h"
#include "reduce.h"

int esperaPorPipeReduce(){
    // se não houver processos zombies, ir esvaziando pipes
    // evitando assim um deadlock quando um filho enche o pipe e o pai fica a
    // espera que ele morra para poder ler a informação e o filho fica a espera
    // que o pai lhe esvazie o pipe
    int i=0;
    for(i=0; i<MAXCOM && !zombieFound; i++){
        lerReduced(i);
    }
    
    int resultado;
    int pid = wait(&resultado);
    zombieFound = false;
    
    if(pid == -1)
        return -1;
    
    if(!WIFEXITED(resultado)){
        // terminou abruptamente
        printErrorAndExit("Uma execução do comando reduce terminou abruptamente.", __FILE__, __LINE__);
    }
    
    bool encontrou = false;
    for(i=0; i<MAXCOM; i++){
        //printf("-----[%d] %d; %d; %d; %d\n", i, pOut[i].pipe[0], pOut[i].pipe[1], pOut[i].pid, pOut[i].vivo);
        if(pOut[i].pid == pid){
            encontrou = true;
            pOut[i].vivo = false;
            break;
        }
    }
    
    if(!encontrou){
        //significa que um filho alimentador terminou
        return esperaPorPipeReduce();
    }
        
    
    return i;
}

int reduce(char *comando){
    char **args;
    separaStrEmArgumentos(comando, &args);
    
    int i=0;
    bool livre = false; //se já iniciou todos os filhos
    int pidAlimentador;
    
    for(i=0; i<MAXCOM; i++){
        pOut[i].pid = -1;
        pOut[i].vivo = false;
    }
    i=0;
    
    int indiceChaveActual = 0;
    
    while(1){
        if( indiceChaveActual  )
        
        if(!livre){
            i = esperaPorPipeReduce();
            if(i == -1)
                printErrorAndExit("Ocorreu um ECHILD no wait antes do tempo.", __FILE__, __LINE__);
        }
        
        
        
        if(pOut[i].vivo == false && pOut[i].pid > 0){
            lerReduced(i);
            close(pOut[i].pipeOut[0]); //fechar o que resta do pipe
            pOut[i].pid = -1;
        }
        
        // atribuir um valor de indice 
        pOut[i].indiceTabela = indiceChaveActual;
        
        // criar um pipe na posição i
        if( pipe(pOut[i].pipeIn) == -1 ){
            if( errno == EMFILE)
               printWarning("Too many file descriptors in use by the process. Retrying in a while.", __FILE__, __LINE__);
            else if( errno == EMFILE)
               printWarning("The system limit on the total number of open files has been reached. Retrying in a while.", __FILE__, __LINE__);
            
            sleep(1000);
        }
        // criar um pipe na posição i para receber o resultado do reduce
        if( pipe(pOut[i].pipeOut) == -1 ){
            if( errno == EMFILE)
               printWarning("Too many file descriptors in use by the process. Retrying in a while.", __FILE__, __LINE__);
            else if( errno == EMFILE)
               printWarning("The system limit on the total number of open files has been reached. Retrying in a while.", __FILE__, __LINE__);
            
            sleep(1000);
        }
        pOut[i].vivo = true;
        
        
        
        // criar filho alimentador
        while( (pidAlimentador = fork()) == -1 ){
            if( errno == EAGAIN )
                printWarning("Não foi possível alocar memória para o filho ou o máximo de filhos permitido foi atingido. Tentando outra vez.", __FILE__, __LINE__);
            if( errno == ENOMEM)
                printWarning("failed to allocate the necessary kernel structures because memory is tight. Retrying in a while.", __FILE__, __LINE__);
            
            sleep(1000);
        }
        
        // tarefa do filho alimentador
        if( pidAlimentador == 0 ){
            // código para o filho alimentador
            close(pOut[i].pipeIn[0]);
            close(pOut[i].pipeOut[1]);
            close(pOut[i].pipeOut[0]);
            
            // alimentar o filho reduce
            listaDumpValues(pOut[i].indiceTabela,pOut[i].pipeIn[1]);
            
            exit(EXIT_SUCCESS);
        }
        
        // criar o filho reduce
        while( (pOut[i].pid = fork()) == -1 ){
            if( errno == EAGAIN )
                printWarning("Não foi possível alocar memória para o filho ou o máximo de filhos permitido foi atingido. Tentando outra vez.", __FILE__, __LINE__);
            if( errno == ENOMEM)
                printWarning("failed to allocate the necessary kernel structures because memory is tight. Retrying in a while.", __FILE__, __LINE__);
            
            sleep(1000);
        }
        
        // executar o reduce
        if( pOut[i].pid==0){
            // código para o filho reduce
            close(pOut[i].pipeIn[1]);
            close(pOut[i].pipeOut[0]);
            
            dup2(pOut[i].pipeIn[0], 0);
            dup2(pOut[i].pipeOut[1], 1);
            
            execvp(args[0], args);
            
            exit(EXIT_FAILURE); //o comando deve terminar naturalmente e nao chegar aqui
        }
        
        // fechar todos os pipes no pai menos o que permite ler o resultado do reduce
        close(pOut[i].pipeIn[0]);
        close(pOut[i].pipeIn[1]);
        
        close(pOut[i].pipeOut[1]);
        
        livre = false;
        for(i=0; i<MAXCOM; i++){
            if(pOut[i].vivo == false){
                livre = true;
                break;
            }
        }
        indiceChaveActual++;
    }
    
    while((i = esperaPorPipeReduce()) != -1){
        lerChaveValor(i);
        close(pOut[i].pipeIn[0]); //fechar o que resta do pipe
        pOut[i].pid = -1; // sinalizar o pipe como terminado
    }
    
    //printTabela();
    listaPrint();
}

int lerReduced(int pos){
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
    
    
    listaReduced(pOut[pos].indiceTabela, buffer);
    
    
    free(buffer);
    return 0;
}

