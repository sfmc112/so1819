#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "biblioteca.h"
#include "client-functions.h"
#include <pthread.h>

void sendLoginToServer(char* user);
void exitClient();
void exitLoginFailure();
void createClientStartingThreads(pthread_t* idEditor, pthread_t* idMyPipe);
void* startEditor();
void* readFromMyPipe();

int fdMyPipe, fdSv;
char user[9];
char myPipe[PIPE_NAME_MAX];
EditorData ed;

int main(int argc, char** argv) {
    fdSv = openNamedPipe(MAIN_PIPE_SERVER, O_WRONLY);

    if (fdSv == -1) {
        fprintf(stderr, "[ERRO]: O pipe principal do servidor, nao esta disponivel!\n");
        return EXIT_FAILURE;
    }

    char tempPipe[PIPE_NAME_MAX];

    strncpy(myPipe, PIPE_USER, PIPE_NAME_MAX);

    //configuraSinal(SIGUSR2);

    checkArgs(argc, argv, myPipe, user);

    createNamedPipe(tempPipe, myPipe);

    strncpy(myPipe, tempPipe, PIPE_NAME_MAX);

    sendLoginToServer(user);

    // Login Efetuado com sucesso
    pthread_t idEditor;
    pthread_t idMyPipe;

    createClientStartingThreads(&idEditor, &idMyPipe);
    pthread_join(idEditor, NULL);
    pthread_join(idMyPipe, NULL);

    exitClient();
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
    strncpy(login.nomePipeCliente, myPipe, PIPE_MAX_NAME);

    int res = write(fdSv, &login, sizeof (login));

    if (res == -1) {
        fprintf(stderr, "[ERRO]: Nao foi enviado o login para o servidor!\n");
        return; // TODO TEM QUE SER ALTERADO
    }

    /*
    
        fdMyPipe = openNamedPipe(myPipe, O_RDONLY);

        ServerMsg msg;

        res = read(fdMyPipe, &msg, sizeof (msg));

        if (res == -1) {
            fprintf(stderr, "[ERRO]: Nao foi possivel ler a resposta do servidor!\n");
            return; // TODO TEM QUE SER ALTERADO
        }

        if (msg.code == LOGIN_FAILURE) {
            printf("Login Falhou!\n");
        }
     */

    closeNamedPipe(fdSv);
}

/**
 * Função responsável por criar as threads.
 * @param idEditor Thread idEditor
 * @param idMyPipe Thread idMyPipe
 */
void createClientStartingThreads(pthread_t* idEditor, pthread_t* idMyPipe) {
    int err;
    err = pthread_create(idEditor, NULL, startEditor, NULL);
    if (err)
        printf("\nNão foi possível criar a thread :[%s]\n", strerror(err));
    else
        printf("\n A thread foi criada!\n");

    err = pthread_create(idMyPipe, NULL, readFromMyPipe, NULL);
    if (err)
        printf("\nNão foi possível criar a thread :[%s]\n", strerror(err));
    else
        printf("\n A thread foi criada!\n");
}

void* startEditor() {
    //TODO mutex
    editor(user, &ed);
}

void* readFromMyPipe() {
    int nBytes;
    ServerMsg msg;
    int serverUp = 1;

    while (serverUp) {
        nBytes = read(fdMyPipe, &msg, sizeof (msg));
        if (nBytes == sizeof (msg)) {
            switch (msg.code) {
                case LOGIN_FAILURE:
                    printf("Login Falhou!\n");
                    exitLoginFailure();
                    break;
                case LOGIN_SUCCESS:
                    fdSv = openNamedPipe(msg.intPipeName, O_WRONLY);
                    if (fdSv == -1) {
                        fprintf(stderr, "[ERRO]: Nao foi possivel abrir pipe interativo <%s> atribuido pelo servidor.\n", msg.intPipeName);
                    } else {
                        exitLoginFailure();
                    }
                    break;
                case SERVER_SHUTDOWN:
                    serverUp = 0;
                    break;
                case EDITOR_UPDATE:
                    // TODO Terá que ser protegido por Mutex
                    ed = msg.ed;
                    // Atualizar Ecra
                    clearEditor(msg.ed.lin, msg.ed.col);
                    break;
                default:
                    break;
            }
        }
    }
}

void exitClient() {
    printf("A aplicação vai encerrar....\n");

    ClientMsg msg;

    msg.msgType = CLIENT_SHUTDOWN;
    strncpy(msg.username, user, 9);

    write(fdSv, &msg, sizeof (msg));

    closeNamedPipe(fdMyPipe);
    closeNamedPipe(fdSv);
    deleteNamedPipe(myPipe);
    exit(0);
}

void exitLoginFailure() {
    printf("A aplicação vai encerrar....\n");
    closeNamedPipe(fdMyPipe);
    closeNamedPipe(fdSv);
    deleteNamedPipe(myPipe);
    exit(0);
}