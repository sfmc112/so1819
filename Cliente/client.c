#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "biblioteca.h"
#include "client-functions.h"

char mainPipe[PIPE_NAME_MAX];
char user[9];

int main(int argc, char** argv) {
    char pipe[PIPE_NAME_MAX];
    strncpy(mainPipe, PIPE_USER, PIPE_NAME_MAX);
    checkArgs(argc, argv, mainPipe, user);
    createNamedPipe(pipe, mainPipe);
    strncpy(mainPipe, pipe, PIPE_NAME_MAX);
    sendLoginToServer(user);
    return (EXIT_SUCCESS);
}
