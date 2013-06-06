/* 
 * Responsável pela fase Map
 */

#pragma once

int map(char *comando);

int lerLinha(char *buffer, int bufferSize);

int lerChaveValor(int pos);

int esperaPorPipeMap();