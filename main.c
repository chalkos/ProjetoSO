#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "comum.h"
#include "map.h"
#include "reduce.h"
#include "erro.h"

int main(int argc, char** argv) {
    if(argc != 3)
        printErrorAndExit("A aplicação deve receber exactamente dois argumentos: map e reduce", __FILE__, __LINE__);
    
    
    MAXCOM = 50;
    /**/
    char* env;
    env = getenv("MAXCOM");
      
    if( env == NULL )
        printErrorAndExit("Variável de ambiente MAXCOM não está definida", __FILE__, __LINE__);
    
    MAXCOM = atoi(env);
    
    if( MAXCOM < 1 )
        printErrorAndExit("MAXCOM tem de ser superior a 0.", __FILE__, __LINE__);
    /**/
    
    gestor = myMalloc(sizeof(Gestor) * MAXCOM);
    
    //printf("comando map: %s\ncomando reduce: %s\n", argv[1], argv[2]);
    
    //assegurar que se um filho morrer chama a função filhoMorreu
    signal(SIGCHLD, filhoMorreu);
    
    zombieFound = false;
    map(argv[1]);
    reduce(argv[2]);
    
    
    return (EXIT_SUCCESS);
}

