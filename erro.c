#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "erro.h"

#define STDERR 2

errorCode lastErrorNr = err_sucesso;
char lastErrorFilename[256];
int lastErrorLine;

char lastErrorString[512];
char lastErrorTime[100];

int setError(errorCode code, int linha, char *nomeFicheiro){
    lastErrorNr = code;
    strcpy(lastErrorFilename, nomeFicheiro);
    lastErrorLine = linha;
    snprintf(lastErrorTime, 100, "%s %s", __TIME__, __DATE__);
    
    printLastError();
    
    return (int)code;
}

errorCode getLastError(){
    return lastErrorNr;
}

void printLastError(){
    switch(lastErrorNr){
        case err_sucesso:
            lastErrorString[0] = '\0';
            break;
        case err_outofmemory:
            snprintf(lastErrorString, 512, "[%s] Out of memory (%s, line #%d)\n",lastErrorTime, lastErrorFilename, lastErrorLine);
            break;
    }
    
    if( lastErrorString[0] != '\0' ){
        write(STDERR, lastErrorString, 512);
    }
}
