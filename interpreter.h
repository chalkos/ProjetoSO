/* 
 * Responsável por interpretar os comandos fornecidos, executa-los e receber
 * informação gerada por eles.
 */

#ifndef INTERPRETER_H
#define	INTERPRETER_H

#define MAXCOM 30

/**
 * Recebe um comando em formato de texto e separa-o em argumentos.
 * @param str O comando em formato de texto
 * @param args Um char** não inicializado onde vão ficar os argumentos. args[0] é o nome do executavel.
 * @return um errorCode com o resultado da operação
 */
int separaStrEmArgumentos(char* str, char** args );

#endif	/* INTERPRETER_H */

