#include <string.h>

#include "interpreter.h"
#include "erro.h"

int separaStrEmArgumentos(char* str, char** args ){
    
    // substitui todos os espaços e parágrafos por \0 e dá um apontador para a posição seguinte
    int i=0;
    args[i] = strtok(str, " \n");
    while(args[i++] != NULL)
        args[i] = strtok(NULL, " \n");
    
    // verifica os argumentos
    if(args[0] == NULL)
        return setError(err_argumentoInvalido, __LINE__, __FILE__);
    
    return err_sucesso;
}
