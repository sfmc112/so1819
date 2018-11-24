#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include "server-commands.h"
#include "server-functions.h"
#include "server-utils.h"
#include "server-users.h"
#include <pthread.h>


int readCommands();
void trataSinal(int numSinal);
void configuraSinal(int sinal);
void createNamedPipesServer(CliPipe* pipes);
void initializeInteractivePipes(int num, CliPipe* pipes);

void joinThreads(pthread_t commands, pthread_t intpipes[]);

EditorData eData;
ServerData sData;

int main(int argc, char** argv) {    
    initializeServerData(&sData);
    checkArgs(argc, argv, &sData);
    
    CliPipe interactivePipes[sData.numInteractivePipes];
    initializeInteractivePipes(sData.numInteractivePipes, interactivePipes);
    
    configuraSinal(SIGUSR1);
    
    createNamedPipesServer(interactivePipes);

    getEnvironmentVariables(&eData, &sData);

    resetMEDITLines(&eData);

    //Inicializar threads;
    pthread_t idCommands;
    pthread_t idIntPipes[sData.numInteractivePipes];
    

    readCommands();
    
    
    joinThreads(idCommands, idIntPipes);
    
    
    //TODO these
    //closePipes(sData, interactivePipes);
    //deletePipes(sData, interactivePipes);

    return (EXIT_SUCCESS);
}

/**
 * Função responsável por ler o comando e interpreta-o.
 * @return 1 se conseguiu e 0 caso contrário
 */
int readCommands() {
    char comando[40]; //TODO alterar para define
    const char* listaComandos[] = {"shutdown", "settings", "load", "save", "free", "statistics", "users", "text"};
    char* token = NULL;
    setbuf(stdout, NULL);
    int i;
    while (1) {
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
                cmdShutdown();
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

void createNamedPipesServer(CliPipe* pipes) {
    char pipeName[PIPE_NAME_MAX];
    createNamedPipe(pipeName, sData.mainPipe);
    strncpy(sData.mainPipe, pipeName, PIPE_NAME_MAX);
    
    char temp[PIPE_NAME_MAX];
    for (int i = 0; i < sData.numInteractivePipes; i++) {
        snprintf(temp, PIPE_NAME_MAX, "%s%d_", INTERACTIVE_PIPE_SERVER, i);
        if(createNamedPipe(pipeName, temp) == 0)
            strncpy(pipes[i].pipeName, pipeName, PIPE_NAME_MAX);
    }
}

void initializeInteractivePipes(int num, CliPipe* pipes){
    int i;
    for(i = 0; i < num; i++){
        pipes[i].numUsers = 0;
    }
}

/*-----------------------THREADS---------------------------------------*/

void joinThreads(pthread_t commands, pthread_t intpipes[]){
    pthread_join(commands, NULL);
    int i;
    for(i = 0; i < sData.numInteractivePipes; i++)
        pthread_join(intpipes[i], NULL);
}

void* receiveNewUsers(){
    
}