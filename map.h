/* 
 * Responsável pela fase Map
 */

#ifndef MAP_H
#define	MAP_H

#include "interpreter.h"

int map(char *comando);

/**
 * Cria um ficheiro com o nome "map###" em que ### é o nr de ficheiro e copia
 * para lá uma linha a partir do stdin. O FD é metido em fdResultante
 * @param nrFicheiro nr do ficheiro a abrir/criar
 * @param fdResultante o descritor do novo ficheiro
 * @return 
 */
int criarFicheiro(int nrFicheiro, int *fdResultante);

/**
 * Lê uma linha do stdin para buffer
 * @param buffer um array de char, previamente alocado
 * @param bufferSize 4096
 * @return -1 se o stdin estiver vazio
 */
int lerLinha(char *buffer, int bufferSize);

int lerChaveValor(int pos);

int esperaPorPipe();

int getNrPipesLivres();

#endif	/* MAP_H */

