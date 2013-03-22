// ficha 4

// fork
#include <unistd.h>

// wait
#include <sys/types.h>
#include <sys/wait.h>


// printf
#include <stdio.h>

// rand
#include <stdlib.h>

// limite de int
#include <limits.h>

/*

fork() - biforca o processo, retorna 0 no processo filho, retorna o pid do filho no processo pai
getpid() - obtém o pid do processo
getppid() - obtém o pid do processo pai

*/


int main(int argc, char* argv[]){
	int matriz[10][10];

	int i,j,stop,procurado = atoi(argv[1]);
	int encontrado[5][3], nencontrados=0;
	int child;
	srand(time(NULL));

	// inicializa a matriz com numeros aleatórios
	for(i=0; i<10; i++)
		for (j = 0; j < 10; j++){
			matriz[i][j] = rand() % 10; //numeros de 0 a 99
			encontrado[i][0] = procurado+1;
		}

	matriz[0][2] = procurado;
	matriz[1][3] = procurado;
	matriz[2][4] = procurado;
	matriz[3][5] = procurado;
	matriz[4][6] = procurado;
	matriz[5][7] = procurado;
	matriz[6][8] = procurado;
	matriz[7][9] = procurado;

	i = j = 0;
	child = fork()==0;
	if(!child){
		i = 10 / 2;
		stop = 10;
	}else
		stop = 10/2;

	while(i<stop){
		for(j=0;j<10;j++){
			if(matriz[i][j] == procurado){
				encontrado[nencontrados][0] = procurado;
				encontrado[nencontrados][1] = i;
				encontrado[nencontrados][2] = j;
				nencontrados++;
				//printf(" encontrou o %d na posicao (%d,%d)!\n", procurado, i, j);
			}
		}
		i++;
	}

	if(child){
		for(i=0; i<nencontrados; i++){
			printf("filho encontrou o %d na posicao (%d,%d)!\n", encontrado[i][0], encontrado[i][1], encontrado[i][2]);
		}
		exit(nencontrados);
	}else{
		
		wait(&stop); //stop passa a ter o numero de encontrados no processo filho
		for(i=0; i<nencontrados; i++){
			printf("  pai encontrou o %d na posicao (%d,%d)!\n", encontrado[i][0], encontrado[i][1], encontrado[i][2]);
		}

		if(WIFEXITED(stop)){
			stop = WEXITSTATUS(stop);

			nencontrados += stop;
			printf("O numero %d ocorreu %d vezes!\n", procurado, nencontrados);
		}else{
			printf("Cópia terminou com erro!\n");
		}
	}
}