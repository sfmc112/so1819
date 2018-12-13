#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "server-commands.h"
#include "server-utils.h"
#include "server-users.h"
#include <pthread.h>

#define MAX_INPUT 255

void* readCommands();
void trataSinal(int numSinal);
void configuraSinal(int sinal);
void createNamedPipesServer(InteractionPipe* pipes);
void openNamedPipesServer(InteractionPipe* pipes);
void initializeInteractivePipes(InteractionPipe* pipes);

void createServerStartingThreads(pthread_t* commands, pthread_t* mainpipe, pthread_t intpipes[], InteractionPipe* pipes);
void* readFromMainPipe(void* arg);
void* readFromIntPipe(void* arg);
void joinThreads(pthread_t mainpipe, pthread_t intpipes[]);
void testaAspell();

EditorData eData;
ServerData sData;
int fdToAspell = -1;
int fdFromAspell = -1;

int fdMainPipe = -1;

int main(int argc, char** argv) {
    if (verifySingleInstance() < 0)
        exitError("[SERVIDOR] Ja existe uma instância do servidor em execucao!");

    initializeServerData(&sData);
    checkArgs(argc, argv, &sData);

    InteractionPipe interactivePipes[sData.numInteractivePipes];
    initializeInteractivePipes(interactivePipes);

    configuraSinal(SIGUSR1);
    configuraSinal(SIGINT);

    createNamedPipesServer(interactivePipes);
    openNamedPipesServer(interactivePipes);

    getEnvironmentVariables(&eData, &sData);

    startAspell(&fdToAspell, &fdFromAspell);

    resetMEDITLines(&eData);

    //Inicializar threads;
    pthread_t idCommands;
    pthread_t idMainPipe;
    pthread_t idIntPipes[sData.numInteractivePipes];

    testaAspell();

    createServerStartingThreads(&idCommands, &idMainPipe, idIntPipes, interactivePipes);

    readCommands();
    
    //Fechar o programa 
/*
    pthread_join(idCommands, NULL);
    printf("[SERVIDOR] A thread responsavel por ler comandos terminou!\n");
*/

    //Fechar o programa
    puts("[SERVIDOR] Vai ser terminado.");

    closeAndDeleteServerPipes(fdMainPipe, &sData, interactivePipes);

    puts("[ASPELL] Vai ser terminado.");
    close(fdToAspell); //vai fazer terminar o Aspell

    joinThreads(idMainPipe, idIntPipes);

    remove("/tmp/unique.txt");
    return (EXIT_SUCCESS);
}

/**
 * Função responsável por ler o comando e interpreta-o.
 * @return 1 se conseguiu e 0 caso contrário
 */
void*readCommands() {
    char comando[MAX_INPUT];
    const char* listaComandos[] = {"shutdown", "settings", "load", "save", "free", "statistics", "users", "text"};
    char* token = NULL;
    setbuf(stdout, NULL);
    int i;
    while (sData.runServer) {
        printf("[SERVIDOR] Introduza o comando: ");
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
                puts("[SERVIDOR] Comando invalido!");
        }
    }
}

/**
 * Função responsável por executar o comportamento de numSinal.
 * @param numSinal Código do sinal.
 */
void trataSinal(int numSinal) {
    if (numSinal == SIGUSR1 || numSinal == SIGINT) {
        puts("Recebi sinal");
        cmdShutdown(&sData);
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
 * @param pipes Array de Pipes Interativo
 */
void createNamedPipesServer(InteractionPipe * pipes) {
    char pipeName[PIPE_NAME_MAX];
    puts("[SERVIDOR] Vai ser criado o pipe principal!");
    createServerNamedPipe(sData.mainPipe);

    char temp[PIPE_NAME_MAX];
    puts("[SERVIDOR] Vao ser criados os pipes de interacao!");
    for (int i = 0; i < sData.numInteractivePipes; i++) {
        snprintf(temp, PIPE_NAME_MAX, "%s%d_", INTERACTIVE_PIPE_SERVER, i);
        if (createNamedPipe(pipeName, temp) == 0)
            strncpy(pipes[i].pipeName, pipeName, PIPE_NAME_MAX);
    }
}

/**
 * Função responsável por abrir os named pipes do servidor.
 * @param pipes Array de Pipes Interativo
 */
void openNamedPipesServer(InteractionPipe * pipes) {
    puts("[SERVIDOR] Vai ser aberto o pipe principal!");
    fdMainPipe = openNamedPipe(sData.mainPipe, O_RDWR);
    puts("[SERVIDOR] Vao ser abertos os pipes de interacao!");
    for (int i = 0; i < sData.numInteractivePipes; i++) {
        pipes[i].fd = openNamedPipe(pipes[i].pipeName, O_RDWR);
    }
}

/**
 * Função responsável por inicializar o número de utilizadores em cada pipe interativo a 0.
 * @param pipes Array de Pipes Interativo
 */
void initializeInteractivePipes(InteractionPipe * pipes) {
    int i;
    puts("[SERVIDOR] A inicializar a informacao relativa aos pipes de interacao...");
    for (i = 0; i < sData.numInteractivePipes; i++) {
        pipes[i].numUsers = 0;
    }
}

/*-----------------------THREADS---------------------------------------*/

/**
 * Função responsável por criar as threads.
 * @param commands Thread para leitura de comandos
 * @param mainpipe Thread para ler do pipe principal do servidor
 * @param intpipes Array dos Pipes Interativos
 */
void createServerStartingThreads(pthread_t* commands, pthread_t* mainpipe, pthread_t intpipes[], InteractionPipe * pipes) {
    puts("[SERVIDOR] Vao ser criadas as threads!");

    int err;
    
    /*
    err = pthread_create(commands, NULL, readCommands, NULL);
    if (err)
        printf("[SERVIDOR] Nao foi possível criar a thread :[%s]\n", strerror(err));
    else
        printf("[SERVIDOR] A thread responsavel pela leitura dos comandos foi criada!\n");
*/

    err = pthread_create(mainpipe, NULL, readFromMainPipe, (void*) pipes);
    if (err)
        printf("[SERVIDOR] Nao foi possível criar a thread :[%s]\n", strerror(err));
    else
        printf("[SERVIDOR] A thread responsavel por ler o pipe principal foi criada!\n");

    int i;
    for (i = 0; i < sData.numInteractivePipes; i++) {
        err = pthread_create((&intpipes[i]), NULL, readFromIntPipe, (void*) &(pipes[i]));
        if (err)
            printf("[SERVIDOR] Nao foi possível criar a thread :[%s]\n", strerror(err));
        else
            printf("[SERVIDOR] A thread responsavel por ler o %d pipe interativo foi criada!\n", i + 1);
    }
}

/**
 * Função responsável por juntar as Threads.
 * @param commands Thread para leitura de comandos
 * @param mainpipe Thread para ler do pipe principal do servidor
 * @param intpipes Array dos Pipes Interativos
 */
void joinThreads(pthread_t mainpipe, pthread_t intpipes[]) {
    pthread_join(mainpipe, NULL);
    printf("[SERVIDOR] A thread responsavel por ler o pipe principal terminou!\n");
    int i;
    for (i = 0; i < sData.numInteractivePipes; i++) {
        pthread_join(intpipes[i], NULL);
        printf("[SERVIDOR] A thread responsavel por ler o %d pipe interativo terminou!\n", i + 1);
    }
}

/**
 * Função responsável por efetuar a leitura do pipe principal do servidor.
 * @return Ponteiro para void (void*)
 */
void* readFromMainPipe(void* arg) {
    int nBytes;
    LoginMsg login;
    ServerMsg msg;
    int fdCli;
    int pos;
    InteractionPipe* pipes = (InteractionPipe*) arg;

    while (sData.runServer) {
        //printf("[SERVIDOR] Estou a espera de input do cliente!\n");
        nBytes = read(fdMainPipe, &login, sizeof (LoginMsg));
        //printf("[SERVIDOR] Recebi informacao!\n");
        if (nBytes == sizeof (LoginMsg)) {
            printf("[SERVIDOR] Recebi login!\n");
            fdCli = openNamedPipe(login.nomePipeCliente, O_WRONLY);
            //printf("\n\n%s\n\n", login.nomePipeCliente);
            if (fdCli == -1)
                continue;
            pos = getFirstAvailablePosition(sData);
            //printf("Pos: %d\n", pos);
            if (!checkUsername(login.username) || checkUserOnline(login.username, sData) || pos == -1) {
                msg.code = LOGIN_FAILURE;
                printf("[SERVIDOR] Falhou o login!\n");
            } else {
                msg.code = LOGIN_SUCCESS;
                int index = getIntPipe(sData, pipes);
                strncpy(msg.intPipeName, pipes[index].pipeName, PIPE_MAX_NAME);
                registerClient(login.username, &sData, pos, fdCli, pipes[index].fd);
                printf("[SERVIDOR] O utilizador %s conectou-se!\n", login.username);
            }
            write(fdCli, &msg, sizeof (msg));
        }
    }
    return NULL;
}

/**
 * Função repsonsável por efetuar a leitura dos pipes interativos. 
 * @param arg
 * @return Ponteiro para void (void*)
 */
void* readFromIntPipe(void* arg) {
    ClientMsg msg;
    int nBytes;
    InteractionPipe* pipeI = (InteractionPipe*) arg;
    while (sData.runServer) {
        nBytes = read(pipeI->fd, &msg, sizeof (msg));
        if (nBytes == sizeof (msg)) {
            switch (msg.msgType) {
                case CLIENT_SHUTDOWN:
                    removeClient(msg.username, &sData);
                    printf("[SERVIDOR] O utilizador %s desconectou-se!\n", msg.username);
                    pipeI->numUsers--;
                    break;
                case K_ENTER:
                    //Ask aspell
                    break;
                default:
                    break;
            }
        }
    }
    return NULL;
}

void testaAspell() {
    puts("[ASPELL] A verificar a frase 'sarahcomh'...");
    if (spellCheckSentence("sarahcomh", fdToAspell, fdFromAspell) == 0) {
        puts("[ASPELL] Esta correto");
    } else {
        puts("[ASPELL] Esta incorreto");
    }
    puts("[ASPELL] A verificar a frase 'Ricardo'...");
    if (spellCheckSentence("Ricardo", fdToAspell, fdFromAspell) == 0) {
        puts("[ASPELL] Esta correto");
    } else {
        puts("[ASPELL] Esta incorreto");
    }
    puts("[ASPELL] A verificar a frase 'Sarah'...");
    if (spellCheckSentence("Sarah", fdToAspell, fdFromAspell) == 0) {
        puts("[ASPELL] Esta correto");
    } else {
        puts("[ASPELL] Esta incorreto");
    }
    puts("[ASPELL] A verificar a frase 'frase do dia'...");
    if (spellCheckSentence("frase do dia", fdToAspell, fdFromAspell) == 0) {
        puts("[ASPELL] Esta correto");
    } else {
        puts("[ASPELL] Esta incorreto");
    }
}