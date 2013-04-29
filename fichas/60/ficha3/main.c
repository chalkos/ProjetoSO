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

#define MAX 100
#define BSIZE 100

typedef struct sFIXE{
	int fd;
	char buffer[BSIZE] = "";
	int porLer = 0;
} FIXE;

int readLine(FIXE fd, char* buffer, int max){
	int lidos = 0;
	char c[BSIZE];
	int n, i;

	while( lidos < max ){

		if(fd.porLer > 0){ // tem cenas no buffer
			for(fd.porLer += 1; fd.porLer<BSIZE; fd.porLer++){
				if(fd.buffer[fd.porLer] == '\n'){
					buffer[lidos] = '\0';
					return lidos;
				}
				buffer[lidos] = fd.buffer[i];
				lidos++;
			}
		}

		if( (n = read(fd, c, BSIZE)) <= 0 ){
			buffer[lidos] = '\0';
			if(lidos == 0)
				return lidos;
			return -1; //fim de ficheiro
		}

		for(i=0; i<BSIZE; i++){
			if(c[i] == '\n'){
				buffer[lidos] = '\0';
				return lidos;
			}
			buffer[lidos] = c[i];
			lidos++;
		}

		
	}
	return lidos;
}


int main(int argc, char* argv[]){
	char buffer[MAX];
	int n;
	FIXE fd;
	fd.fd = open(argv[1], O_RDONLY);
	

	// se ocorrer um erro
	if( fd.fd < 0){
		perror(argv[1]);
		return 1;
	}

	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);
	readLine(fd, buffer, MAX);
	printf("linha: %s\n", buffer);

	/*
	// ler enquanto o numero de caracteres lido for maior que 0
	while( (n = read(fd, buffer, MAX)) > 0 )
		write(1, buffer, n); // escrever no STDOUT, n caracteres
		*/

	close(fd);
	return 0;
}