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

PIPES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

0 - saída do pipe (de onde se deve ler)
1 - entrada do pipe (onde se deve escrever)

lsof -p pidProcesso -d 0-10     <<<<<<<<<<<<<<<<<<<<<<< descritores de ficheiros

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
	char *cmds[200];
	int cmdi = 0;

	// redireccionamento
	char *stdoutFile=NULL;
	char *stdinFile=NULL;
	int fd;

	int antes[2];  // pipe anterior
	int depois[2]; // pipe seguinte

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
		cmdi = 0;

		cmds[cmdi] = strtok(input, "|");
		while(cmds[cmdi++]!=NULL)
			cmds[cmdi] = strtok(NULL, "|");


		pipe(depois);


		for(j=0; j<cmdi; j++){



			// criar um novo processo
			if(fork()==0){
				// um filho nunca escreve no antes
				if(j > 0)
					close(antes[1]);

				// um filho nunca lê do depois
				if(j < cmdi-1)
					close(depois[0]);


				// cada processo vai ler a sua lista de argumentos
				args[i] = strtok(cmds[j], " \n");
				while(args[i++]!=NULL)
					args[i] = strtok(NULL, " \n");


				if(j > 0)
					dup2(antes[0],0);

				if(j < cmdi-1)
					dup2(depois[1],1);

				execvp(args[0], args);
				perror("ERROR");

				exit(1);
			}else{
				// fechar os dois pipes (nao vao ser usados pelo pai)
				if(j > 0){
					close(antes[0]);
				}
				
				close(depois[1]);

				// mudar os pipes para o proximo processo
				antes[0] = depois[0];


				// criar o novo
				if(j < cmdi-1)
					pipe(depois);



			}
		}
	}
	printf("feito :D\n");
	exit(EXIT_SUCCESS);
}