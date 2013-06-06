#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "erro.h"

char errorLineAsString[10];

void printErrorAndExit(const char* erro, const char* file, int line){
    write(2, "[ERRO] ", 7);
    write(2, erro, strlen(erro));
    write(2, "(", 1);
    write(2, file, strlen(file));
    write(2, "@line", 5);
    
    snprintf(errorLineAsString, 10, "%d", line);
    
    write(2, errorLineAsString, 10);
    write(2, ")\nA aplicaçao vai terminar.\n", 29);
    exit(EXIT_FAILURE);
}

void printWarning(const char* erro, const char* file, int line){
    write(2, "[WARN] ", 7);
    write(2, erro, strlen(erro));
    write(2, "(", 1);
    write(2, file, strlen(file));
    write(2, "@line", 5);
    
    snprintf(errorLineAsString, 10, "%d", line);
    
    write(2, errorLineAsString, 10);
    write(2, ")\n", 2);
}
