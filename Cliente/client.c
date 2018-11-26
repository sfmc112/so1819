#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "biblioteca.h"
#include "client-functions.h"

void sendLoginToServer(char* user);


int fdMyPipe, fdSv;
char user[9];
char mainPipe[PIPE_NAME_MAX];

int main(int argc, char** argv) {
    fdSv = openNamedPipe(MAIN_PIPE_SERVER, O_WRONLY);

    if (fdSv == -1) {
        fprintf(stderr, "[ERRO]: O pipe principal do servidor, nao esta disponivel!\n");
        return EXIT_FAILURE;
    }

    char tempPipe[PIPE_NAME_MAX];

    strncpy(mainPipe, PIPE_USER, PIPE_NAME_MAX);

    //configuraSinal(SIGUSR2);

    checkArgs(argc, argv, mainPipe, user);

    createNamedPipe(tempPipe, mainPipe);

    strncpy(mainPipe, tempPipe, PIPE_NAME_MAX);

    sendLoginToServer(user);

    closeNamedPipe(fdMyPipe);
    closeNamedPipe(fdSv);
    deleteNamedPipe(mainPipe);
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

/**
 * Função responsável por enviar o username para o servidor. Fazendo com que o mesmo seja validado.
 * @param login username do cliente
 */
void sendLoginToServer(char* user) {
    LoginMsg login;
    strncpy(login.username, user, 9);
    strncpy(login.nomePipeCliente, mainPipe, PIPE_MAX_NAME); 
    
    int res = write(fdSv, &login, sizeof(login));

    if (res == -1) {
        fprintf(stderr, "[ERRO]: Nao foi enviado o login para o servidor!\n");
        return; // TODO TEM QUE SER ALTERADO
    }

    fdMyPipe = openNamedPipe(mainPipe, O_RDONLY);

    ServerMsg msg;

    res = read(fdMyPipe, &msg, sizeof (msg));

    if (res == -1) {
        fprintf(stderr, "[ERRO]: Nao foi possivel ler a resposta do servidor!\n");
        return; // TODO TEM QUE SER ALTERADO
    }

    if (msg.code == LOGIN_FAILURE) {
        printf("Login Falhou! A aplicação vai encerrar....\n");
    } else {
        // Login Efetuado com sucesso
        editor(user);
    }
}