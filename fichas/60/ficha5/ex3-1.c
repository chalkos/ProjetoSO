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



int main(int argc, char* argv[]){
	int iArg = 1;

	while(iArg < argc){
		// criar um novo processo
		if(fork()==0){
			// filho

			//obter um array de argumentos
			//char *Args[][];

			execlp(argv[iArg], argv[iArg], NULL);
			perror("Erro ao iniciar a aplicação");

			exit(1);
		}else{
			// pai
			wait(NULL);
			printf("**************************************\n");
			iArg++;
		}
	}
	printf("feito :D\n");
}