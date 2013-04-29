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


int main(int argc, char* argv[]){
	int iArg = 1;
	int i = 0;
	char input[250] = "";
	char *args[200];


	while(1){
		// ler um comando
		printf(">> ");
		if( fgets(input, 249, stdin) == NULL )
			break;

		i = 0;
		args[i] = strtok(input, " \n");
		while(args[i++]!=NULL)
			args[i] = strtok(NULL, " \n");


		// comando exit
		if(strcmp(input, "exit") == 0)
			break;

		// cd
		if( strcmp(args[0], "cd") == 0){
			//execvp(args[0], args);
			if( chdir(args[1]) == -1)
				perror("ERROR");
			continue;
		}

		// criar um novo processo
		if(fork()==0){
			// filho

			execvp(args[0], args);
			perror("ERROR");

			exit(1);
		}else{
			// pai
			wait(NULL);
		}
	}
	printf("feito :D\n");
	exit(EXIT_SUCCESS);
}