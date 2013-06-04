/* 
 * Responsável pela fase Map
 */

#pragma once

int map(char *comando);

/**
 * Lê uma linha do stdin para buffer
 * @param buffer um array de char, previamente alocado
 * @param bufferSize 4096
 * @return -1 se o stdin estiver vazio
 */
int lerLinha(char *buffer, int bufferSize);

int lerChaveValor(int pos);

int esperaPorPipeMap();

int getNrPipesLivres();

/**
 * Chamada quando um filho morre
 */
void filhoMorreu();