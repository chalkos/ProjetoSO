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

// exec
#include <unistd.h>

// fork
#include <unistd.h>

// wait
#include <sys/types.h>
#include <sys/wait.h>


// printf
#include <stdio.h>

// rand
#include <stdlib.h>

// strtok
#include <string.h>

// signal
#include <signal.h>

/*

fork() - biforca o processo, retorna 0 no processo filho, retorna o pid do filho no processo pai
getpid() - obtém o pid do processo
getppid() - obtém o pid do processo pai

*/

#define MX 100



int pidFilhos[MX];
int resFilhos[MX];
int sair = 0;
int nencontrados=0;
int fn = 0;

void CopiaEncontrou(){
	int fn;
	for(fn = 0; fn < MX; fn++){
		if(pidFilhos[fn] != 0){
			kill(pidFilhos[fn],SIGTERM); //matar todos os outros filhos
		}
	}
	sair = 1;
	nencontrados = 1;
}

void VaiFechar(){
	printf("fechando o nr %d\n", fn);
	exit(0);
}

int main(int argc, char* argv[]){
	int matriz[MX][MX];

	int i,j,stop,procurado = atoi(argv[1]);
	
	int encontrado[5][3];
	for(i=0; i<MX; i++){
		resFilhos[i] = 0;
	}
	int child;
	srand(time(NULL));

	signal(SIGUSR1, CopiaEncontrou);
	signal(SIGTERM, VaiFechar);


	// inicializa a matriz com numeros aleatórios
	for(i=0; i<MX; i++)
		for (j = 0; j < MX; j++){
			matriz[i][j] = rand() % 100; //numeros de 0 a 99
			encontrado[i][0] = procurado+1;
		}

	i = j = 0;


	// apenas ter filhos a procurar
	for(fn = 0; fn < MX; fn++){
		if(sair == 1) break;

		pidFilhos[fn] = fork();
		if(pidFilhos[fn] == 0){ //está no filho número "fn"
			for(i=0; i<MX; i++){
				if(matriz[fn][i] == procurado){
					printf("filho: %d; posicao (%d,%d)\n", fn, fn, i);
					nencontrados++;
					kill(getppid(), SIGUSR1);
					//exit(nencontrados);
				}
			}
			printf("fechando naturalmente o nr %d\n", fn);
			exit(nencontrados);
		}
	}

	while( wait(NULL) > 0)
		;
/*
	for(fn=0; fn<MX; fn++){
		if(WIFEXITED(resFilhos[fn])){
			resFilhos[fn] = WEXITSTATUS(resFilhos[fn]);

			nencontrados += resFilhos[fn];
			printf("Cópia nr %d encontrou o numero %d vezes!\n", fn, resFilhos[fn]);
		}else{
			printf("Cópia nr %d terminou com erro!\n", fn);
		}
	}*/
	printf("No total o numero foi encontrado %d vezes!", nencontrados);
}