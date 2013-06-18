// para pipes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

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

#define MX 10



int pidFilhos[MX];
int resFilhos[MX];
int sair = 0;
int nencontrados=0;
int fn = 0;

int main(int argc, char* argv[]){
	int matriz[MX][MX];

	int i,j,stop;
	
	int encontrado[MX];
	int encontrado_filho[MX];
	int fd[2];

	int child;
	
	srand(time(NULL));

	pipe(fd);


	// inicializa a matriz com numeros aleatórios
	for(i=0; i<MX; i++){
		encontrado[i] = 0;
		for (j = 0; j < MX; j++){
			matriz[i][j] = rand() % 2; //numeros de 0 a 99
			//encontrado[i][0] = procurado+1;
		}
	}

	i = j = 0;


	// apenas ter filhos a procurar
	for(fn = 0; fn < MX; fn++){

		pidFilhos[fn] = fork();
		if(pidFilhos[fn] == 0){ //está no filho número "fn"
			close(fd[0]);
			write(fd[1], &matriz[fn][0], sizeof(int)*MX);
			//printf("fechando naturalmente o nr %d\n", fn);
			exit(EXIT_SUCCESS);
		}
	}

	close(fd[1]);
	int n;
	while( (n=read(fd[0], &encontrado_filho, sizeof(int)*MX)) > 0 ){

		for(i=0; i<MX; ++i){
			encontrado[i] += encontrado_filho[i];
		}
	}

	printf("feito, resultado:\n");
	for(i=0; i<MX; ++i){
		printf("res[%d] = %d\n", i, encontrado[i]);
	}

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
//	printf("No total o numero foi encontrado %d vezes!", nencontrados);
}