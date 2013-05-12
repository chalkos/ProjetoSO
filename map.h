/* 
 * Responsável pela fase Map
 */

#ifndef MAP_H
#define	MAP_H

int map(char *comando);

/**
 * Cria um ficheiro com o nome "map###" em que ### é o nr de ficheiro e copia
 * para lá uma linha a partir do stdin. O FD é metido em fdResultante
 * @param nrFicheiro nr do ficheiro a abrir/criar
 * @param fdResultante o descritor do novo ficheiro
 * @return 
 */
int criarFicheiro(int nrFicheiro, int *fdResultante);

#endif	/* MAP_H */

