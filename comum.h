/* 
 * Responsável por interpretar os comandos fornecidos, executa-los e receber
 * informação gerada por eles.
 */

#pragma once

#include "bool.h"

#define MAXCOM 5

typedef struct sPipes{
   int pipeIn[2];   // 0 = read, 1 = write
   int pipeOut[2];
   int pid;
   bool vivo; // se o processo está a correr ou se já terminou
   int indiceTabela;
} pipes;

bool zombieFound;

//informações sobre os filhos e pipes
pipes pOut[MAXCOM];

/**
 * Recebe um comando em formato de texto e separa-o em argumentos.
 * @param str O comando em formato de texto
 * @param args Um char** não inicializado onde vão ficar os argumentos. args[0] é o nome do executavel.
 */
void separaStrEmArgumentos(char* str, char*** args );

void filhoMorreu();

void *myMalloc(size_t size);
void *myRealloc(void *ptr, size_t size);

