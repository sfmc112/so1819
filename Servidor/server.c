#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "server-commands.h"
#include "server-functions.h"
#include "server-utils.h"
#include "server-users.h"
#include <pthread.h>


void* readCommands();
void trataSinal(int numSinal);
void configuraSinal(int sinal);
void createNamedPipesServer(InteractionPipe* pipes);
void openNamedPipesServer(InteractionPipe* pipes);
void initializeInteractivePipes(InteractionPipe* pipes);

void createServerStartingThreads(pthread_t* commands, pthread_t* mainpipe, pthread_t intpipes[]);
void* readFromMainPipe();
void* readFromClientPipe();
void joinThreads(pthread_t commands, pthread_t mainpipe, pthread_t intpipes[]);

EditorData eData;
ServerData sData;

int fdMainPipe;

int main(int argc, char** argv) {
    if (verifySingleInstance() < 0)
        exitError("Já existe uma instância do servidor em execução!");

    initializeServerData(&sData);
    checkArgs(argc, argv, &sData);

    InteractionPipe interactivePipes[sData.numInteractivePipes];
    initializeInteractivePipes(interactivePipes);
    openNamedPipesServer(interactivePipes);

    configuraSinal(SIGUSR1);

    createNamedPipesServer(interactivePipes);
    openNamedPipesServer(interactivePipes);

    getEnvironmentVariables(&eData, &sData);

    resetMEDITLines(&eData);

    //Inicializar threads;
    pthread_t idCommands;
    pthread_t idMainPipe;
    pthread_t idIntPipes[sData.numInteractivePipes];

    createServerStartingThreads(&idCommands, &idMainPipe, idIntPipes);

    //Fechar o programa

    joinThreads(idCommands, idMainPipe, idIntPipes);
    
    //Fechar o programa
    printf("O servidor vai terminar!\n");
    //TODO these
    //closePipes(interactivePipes);
    //deletePipes(interactivePipes);

    return (EXIT_SUCCESS);
}

/**
 * Função responsável por ler o comando e interpreta-o.
 * @return 1 se conseguiu e 0 caso contrário
 */
void* readCommands() {
    char comando[40]; //TODO alterar para define
    const char* listaComandos[] = {"shutdown", "settings", "load", "save", "free", "statistics", "users", "text"};
    char* token = NULL;
    setbuf(stdout, NULL);
    int i;
    while (sData.runServer) {
        printf("Introduza o comando: ");
        scanf(" %39[^\n]", comando);
        //comando tudo em letras minusculas
        toLower(comando);
        //1ª parte do comando
        token = strtok(comando, " ");
        //Imprime comando
        //printf("%s\n", token);
        for (i = 0; i < 8 && strcmp(listaComandos[i], token) != 0; i++)
            ;
        switch (i) {
            case 0:
                cmdShutdown(&sData);
                break;
            case 1:
                cmdSettings(sData, eData);
                break;
            case 2:
                if (checkCommandArgs(token))
                    cmdLoad();
                break;
            case 3:
                if (checkCommandArgs(token))
                    cmdSave();
                break;
            case 4:
                if (checkCommandArgs(token))
                    cmdFree();
                break;
            case 5:
                cmdStats();
                break;
            case 6:
                cmdUsers();
                break;
            case 7:
                cmdText();
                break;
            default:
                puts("Comando invalido!");
        }
    }
}

/**
 * Função responsável por executar o comportamento de numSinal.
 * @param numSinal Código do sinal.
 */
void trataSinal(int numSinal) {
    if (numSinal == SIGUSR1) {
        exitNormal();
    }
}

/**
 * Função responsável por redefinir o comportamento de sinal.
 * @param sinal o sinal que o programa recebeu
 */
void configuraSinal(int sinal) {
    if (signal(sinal, trataSinal) == SIG_ERR) {
        exitError("Erro a tratar sinal!");
    }
}

/**
 * Função responsável por criar os named pipes do servidor.
 */
void createNamedPipesServer(InteractionPipe* pipes) {
    char pipeName[PIPE_NAME_MAX];
    createServerNamedPipe(sData.mainPipe);

    char temp[PIPE_NAME_MAX];
    for (int i = 0; i < sData.numInteractivePipes; i++) {
        snprintf(temp, PIPE_NAME_MAX, "%s%d_", INTERACTIVE_PIPE_SERVER, i);
        if (createNamedPipe(pipeName, temp) == 0)
            strncpy(pipes[i].pipeName, pipeName, PIPE_NAME_MAX);
    }
}

void openNamedPipesServer(InteractionPipe* pipes) {
    fdMainPipe = openNamedPipe(sData.mainPipe, O_RDWR);

    for (int i = 0; i < sData.numInteractivePipes; i++) {
        pipes[i].fd = openNamedPipe(pipes[i].pipeName, O_RDWR);
    }
}

void initializeInteractivePipes(InteractionPipe* pipes) {
    int i;
    for (i = 0; i < sData.numInteractivePipes; i++) {
        pipes[i].numUsers = 0;
    }
}

/*-----------------------THREADS---------------------------------------*/

void createServerStartingThreads(pthread_t* commands, pthread_t* mainpipe, pthread_t intpipes[]) {
    int err;
    err = pthread_create((commands), NULL, readCommands, NULL);
    if (err)
        printf("\nNão foi possível criar a thread :[%s]", strerror(err));
    else
        printf("\n A thread foi criada!\n");

    err = pthread_create((mainpipe), NULL, readFromMainPipe, NULL);
    if (err)
        printf("\nNão foi possível criar a thread :[%s]", strerror(err));
    else
        printf("\n A thread foi criada!\n");

    int i;
    for (i = 0; i < sData.numInteractivePipes; i++) {
        err = pthread_create((&intpipes[i]), NULL, readFromClientPipe, NULL);
        if (err)
            printf("\nNão foi possível criar a thread :[%s]", strerror(err));
        else
            printf("\n A thread foi criada!\n");
    }
}

void joinThreads(pthread_t commands, pthread_t mainpipe, pthread_t intpipes[]) {
    pthread_join(commands, NULL);
    pthread_join(mainpipe, NULL);
    int i;
    for (i = 0; i < sData.numInteractivePipes; i++)
        pthread_join(intpipes[i], NULL);
}

void* readFromMainPipe() {
    int nBytes;
    
    while (sData.runServer) {
        nbytes = read(fdMainPipe, 
    }
}

void* readFromClientPipe() {
    //TODO por acabar
    while (sData.runServer) {
        printf("Fake read from client pipe...\n");
        sleep(8);
    }
}