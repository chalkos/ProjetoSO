#include <stdio.h>
#include <stdlib.h>

#include "interpreter.h"
#include "map.h"
#include "reduce.h"
#include "profiler.h"
#include "erro.h"

/*
 * 
 */
int main(int argc, char** argv) {
    if(argc != 3){
        fprintf(stderr, "A aplicação deve receber exactamente dois argumentos: map e reduce\n");
        return (EXIT_FAILURE);
    }
    
    printf("comando map: %s\ncomando reduce: %s\n", argv[1], argv[2]);
    
    
    map(argv[1]);
    reduce(argv[2]);
    
    
    return (EXIT_SUCCESS);
}

