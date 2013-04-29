#include <stdio.h>
#include <stdlib.h>


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
Posições do array fd (usado para pipes)
		stdin - 0 -> read da posição 0
		stdout - 1 -> write da posição 1
		stderr - 2 -> X


READ
	há dados ===> return maior que 0
	n há dados ===>
		há escritores - bloqueia
		não há escritores - return <= 0 (EOF)

WRITE
	há leitores ===>
		há espaço ===> escreve
		não há espaço ===> espera
	não há leitores ===> vai receber um SIGPIPE
	*/

int main(int argc, char* argv[]){
	int i = 0;
	int fd[2]; // 0 onde se lê, 1 onde se escreve

	pipe(fd);

	if(fork()==0){
		// filho
		close(fd[0]); // <------------ IMPORTANTE
		i+=3;

		write(fd[1], &i, sizeof(int));
		exit(0);
	}else{
		close(fd[1]); // <------------ IMPORTANTE
		i+=4;
	}
	
	int i_filho;
	read(fd[0], &i_filho, sizeof(int));
	
	i += i_filho;

	printf("%d\n", i);

	exit(EXIT_SUCCESS);
}