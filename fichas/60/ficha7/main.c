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

*/

// precisos para as flags do open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


// função a ser invocada quando chegar determinado sinal
// signal( _sinal_ , _acção_ );
void dontDie(){
	printf("I will survive! >> ");
}

void killOnRedimension(){
	kill(getpid(), SIGKILL);
}

void dieZombieDie(){
	wait(NULL);
}

int main(int argc, char* argv[]){
	int iArg = 1;
	int i = 0;
	int j;
	int background = 0;
	char input[250] = "";
	char *args[200];

	// redireccionamento
	char *stdoutFile=NULL;
	char *stdinFile=NULL;
	int fd;

	//signal(SIGINT, dontDie);
	//signal(SIGWINCH, killOnRedimension);
	signal(SIGCHLD, dieZombieDie);

	while(1){
		// ler um comando
		printf(">> ");
		if( fgets(input, 249, stdin) == NULL )
			break;

		background = 0;
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

		// lidar com o redireccioador >
		j=0;
		for(j=0; j < i-1; j++){
			if(strcmp(args[j], ">") == 0){
				stdoutFile = args[j+1];
				i = j+1;
				args[j] = NULL;
			}
		}

		// lidar com redireccionador <
		j=0;
		for(j=0; j < i-1; j++){
			if(strcmp(args[j], "<") == 0){
				stdinFile = args[j+1];
				i = j+1;
				args[j] = NULL;
			}
		}

		if( i > 2 && strcmp(args[i-2], "&") == 0 ){
			//printf("background\n");
			args[i-2] = NULL;
			background = 1;
		}


		// criar um novo processo
		if(fork()==0){
			// filho


			// dar lhe um input especifico
			if(stdinFile != NULL){
				fd = open(stdinFile, O_CREAT|O_RDONLY, 0644);
				dup2(fd, 0);
				close(fd);
			}

			// dar um output especifico
			if(stdoutFile != NULL){
				fd = open(stdoutFile, O_CREAT|O_WRONLY, 0644);
				dup2(fd, 1);
				dup2(fd, 2);
				close(fd);
			}

			execvp(args[0], args);
			perror("ERROR");

			exit(1);
		}else{
			stdinFile = NULL;
			stdoutFile = NULL;

			// pai
			if(!background) // se for foreground, esperar
				wait(NULL);
		}
	}
	printf("feito :D\n");
	exit(EXIT_SUCCESS);
}