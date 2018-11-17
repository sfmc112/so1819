#include <stdio.h>
#include <stdlib.h>
#include "client-functions.h"
#include "biblioteca.h"
/*
 * 
 */
int main(int argc, char** argv) {
    // TODO AASDSADASDSADSADA
    checkArgs(argc, argv);
    char pipeName[PIPE_NAME_MAX];
    printf("caching");
    printf("%d\n",createNamedPipe(pipeName, PIPE_USER));
    printf("%s\n", pipeName);
    deleteNamedPipe(pipeName);
    return (EXIT_SUCCESS);
}

