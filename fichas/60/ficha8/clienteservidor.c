// exec
#include <unistd.h>

// fork
#include <unistd.h>

// wait
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>



// printf
#include <stdio.h>

// rand
#include <stdlib.h>

// strtok
#include <string.h>

// signal
#include <signal.h>

/*
Chamadas ao sistema
#include <signal.h>
#include <sys/types.h>

typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler);
	função a ser chamada quando se recebe determinado comando

int kill(pid_t pid, int sig);
	envia um sinal ao processo
	não podemos enviar sinais a processos de outros utilizadores

unsigned int alarm(unsigned int seconds);


int pause(void);


*/

/*
MANUAL:
	man 2 signal - utilização da função signal e sinais especiais
	man 7 signal - lista de sinais

SINAIS IMPORTANTES:
	SIGINT - CTRL+C
	SIGTERM - fazer kill ao processo
	SIGKILL - sinal especial que não pode ser associado a uma função, serve sempre para matar o processo de vez
				isto é o mesmo que usar "kill -9 nome_processo"
	SIGSEGV - sinal SEGmentation Violation ... No more segmentation fault, yey!
	SIGCHLD - sinal que é enviado ao pai, sempre que o filho termina (permite que o pai execute qualquer coisa quando o filho termina)
	SIGWINCH - sinal que ajuda o processo a re-adaptar-se ao tamanho da janela

	SIGUSR1 / SIGUSR2 - sinais reservados, o sistema operativo nunca usa estes. Servem para o utilizador os enviar e tratar

	NOVO (26/04/2013):
	SIGPIPE - enviado quando se tenta escrever num pipe que não tem ninguém à escuta

SINAIS ESPECIAIS:
	SIG_IGN - ignora a definição desse sinal
	SIG_DFC - repõe a definição desse sinal
*/

/* REDIRECCIONAMENTO <<<<<<<<<<<<<<<<<<<<<<<

0 - stdin
1 - stdout
2 - stderr

PIPES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

0 - saída do pipe (de onde se deve ler)
1 - entrada do pipe (onde se deve escrever)

lsof -p pidProcesso -d 0-10     <<<<<<<<<<<<<<<<<<<<<<< descritores de ficheiros

NAMED PIPES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
mkfifo("nome_ficheiro", permissões);
fd[0] = open("nome_ficheiro", O_WRONLY);
fd[1] = open("nome_ficheiro", O_RDONLY);

*/

// precisos para as flags do open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PNOME "pipe.txt"

void server(){
	printf(":::SERVIDOR:::\n");
	mkfifo(PNOME, 0600);
	int pipe = open(PNOME, O_RDONLY);
	int lidos;
	char lido;
	while( read(pipe, &lido, sizeof(char)) > 0 ){
		printf("%c", lido);
	}
}

void client(){
	printf(":::CLIENTE:::\n");
	int pipe = open(PNOME, O_WRONLY);
	char string[512] = "";
	while(1){
		fgets(string, 512, stdin); // ler do stdin
		write(pipe, string, 512);
	}
}

int main(int argc, char* argv[]){
	if( argc != 2){
		printf("Escolha cliente (-c) ou servidor (-s).\n");
		exit(EXIT_FAILURE);
	}

	if( strcmp(argv[1], "-s") == 0){
		server();
	}else if( strcmp(argv[1], "-c") == 0){
		client();
	}else{
		printf("Parametro desconhecido. Escolha cliente (-c) ou servidor (-s).\n");
	}
}