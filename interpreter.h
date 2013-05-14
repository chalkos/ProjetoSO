/* 
 * Responsável por interpretar os comandos fornecidos, executa-los e receber
 * informação gerada por eles.
 */

#ifndef INTERPRETER_H
#define	INTERPRETER_H

#include "bool.h"

#define MAXCOM 5

typedef struct sPipes{
   int pipe[2];   // 0 = read, 1 = write
   int pid;
   bool vivo; // se o processo está a correr ou se já terminou
} pipes;

typedef struct sTabela{
    char *chave;
    char *valor;
} Tabela;

Tabela *tabela;

/**
 * Recebe um comando em formato de texto e separa-o em argumentos.
 * @param str O comando em formato de texto
 * @param args Um char** não inicializado onde vão ficar os argumentos. args[0] é o nome do executavel.
 * @return um errorCode com o resultado da operação
 */
int separaStrEmArgumentos(char* str, char*** args );

int inicializaTabela();

int aumentaTabela();

int insereNaTabela(char *chave, char *valor);

int printTabela();

int agrupaChavesTabela();

#endif	/* INTERPRETER_H */

