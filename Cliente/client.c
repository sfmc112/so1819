#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "biblioteca.h"
#include "client-functions.h"

void sendLoginToServer(char* login);


int fdMyPipe, fdSv;
char user[9];
char mainPipe[PIPE_NAME_MAX];

int main(int argc, char** argv) {
    char tempPipe[PIPE_NAME_MAX];

    strncpy(mainPipe, PIPE_USER, PIPE_NAME_MAX);

    //configuraSinal(SIGUSR2);

    checkArgs(argc, argv, mainPipe, user);

    createNamedPipe(tempPipe, mainPipe);

    strncpy(mainPipe, tempPipe, PIPE_NAME_MAX);

    sendLoginToServer(user);

    return (EXIT_SUCCESS);
}

/**
 * Função responsável por executar o comportamento de numSinal.
 * @param numSinal Código do sinal.
 */

/*
void trataSinal(int numSinal) {
    if (numSinal == SIGUSR2) {
        closeNamedPipe(fdMyPipe);
        closeNamedPipe(fdSv);
        deleteNamedPipe(mainPipe);
        exit(-1); // TODO ALTERAR
    }
}
 */

void sendLoginToServer(char* login) {
    fdSv = openNamedPipe(MAIN_PIPE_SERVER, O_WRONLY);

    if (fdSv == -1) {
        fprintf(stderr, "[ERRO]: O pipe principal do servidor, nao esta disponivel!\n");
        return;
    }

    int res = write(fdSv, login, strlen(user));

    if (res == -1) {
        fprintf(stderr, "[ERRO]: Nao foi enviado o login para o servidor!\n");
        return; // TODO TEM QUE SER ALTERADO
    }

    fdMyPipe = openNamedPipe(mainPipe, O_RDONLY);

    ServerMsg msg;

    res = read(fdMyPipe, &msg, sizeof (ServerMsg));

    if (res == -1) {
        fprintf(stderr, "[ERRO]: Nao foi possivel ler a resposta do servidor!\n");
        return; // TODO TEM QUE SER ALTERADO
    }

    if (msg.code == LOGIN_FAILURE) {
        // Username incorreto
        closeNamedPipe(fdMyPipe);
        closeNamedPipe(fdSv);
        deleteNamedPipe(mainPipe);
    } else {
        // Login Efetuado com sucesso
        editor(user);
    }
}
