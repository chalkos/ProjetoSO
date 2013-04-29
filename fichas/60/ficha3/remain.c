/*
Em C:
	FILE*           stdin  stdout  stderr
		fopen
		fgets
		fscanf
		fprintf

Em Unix:
	int               0       0       0
		open
		read
		write
		close

	exemplo: read 0 => ler do ficheiro
			 write 1 => escrever no ficheiro

Re-escrever o comando cat
*/

// open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// close & write
#include <unistd.h>

// printf
#include <stdio.h>

#define MAX 200
#define BSIZE 105

typedef struct sFicheiro{
	int fd;
	char buffer[BSIZE];
	int i;
} Ficheiro;

int readLine(Ficheiro *fd, char* out, int max){
	int i=0;
	int n;

	while(1){

			//printf("*%d-%d*\n",fd->i,n);

		// verifica se o buffer está vazio e se estiver, carrega dados
		if(fd->i >= BSIZE){
			n = read(fd->fd, fd->buffer, BSIZE);

//			printf("*%d-%d*",fd->i,n);

			if(n <= 0 && i == 0) // ainda nao passou nada do buffer para out, e não conseguiu carregar nada do ficheiro => EOF!!!
				return -1; // >>>>>>>>>>>>>>>> EOF
			
			fd->i = 0;
			if(n <= 0){
				// >>>>>> bug? incrementar o i? meter \0?
				return i;  // >>>>>>>>>>>>>>>> EOF, mas leu qualquer coisa
			}
		}


		// lê um caracter do buffer para o out
		if(i < max-1){ // ainda pode escrever para out
			out[i] = fd->buffer[fd->i];

			printf("pos%d:%d***\n", i, out[i]);

			i++;
			fd->i++;
			if(out[i-1] == '\n'){
				out[i-1] = '\0';
				return i;  //>>>>>>>>>>>>>>>>> Encontrou \n, substitui por \0 para terminar string
			}
		}else{ // já não pode escrever para out
			out[max-1] = '\0'; //terminar a string
			return max-1;
		}

	}
}


int main(int argc, char* argv[]){
	char buffer[MAX];
	int n;
	Ficheiro fd;
	fd.fd = open(argv[1], O_RDONLY);
	fd.i = BSIZE;
	

	// se ocorrer um erro
	if( fd.fd < 0){
		perror(argv[1]);
		return 1;
	}

	//readLine(&fd, buffer, MAX);


	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';
	printf(">>>>>>>>>>>>leu %d: %s\n", readLine(&fd, buffer, MAX), buffer); buffer[0] = '\0';

	/*
	// ler enquanto o numero de caracteres lido for maior que 0
	while( (n = read(fd, buffer, MAX)) > 0 )
		write(1, buffer, n); // escrever no STDOUT, n caracteres
		*/

	close(fd.fd);
	return 0;
}