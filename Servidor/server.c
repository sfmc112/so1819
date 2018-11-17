#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <bits/fcntl-linux.h>
#include "server-commands.h"
#include "server-functions.h"
#include "server-utils.h"
#include "server-users.h"
#include "biblioteca.h"

int readCommands();
void trataSinal(int numSinal);
void configuraSinal(int sinal);

EditorData eData;
ServerData sData;

void testeSelect(char* pipeName) {
    fd_set read_fd_set;
    FD_ZERO(&read_fd_set);
    FD_SET(1, &read_fd_set);
    int fd = openNamedPipe(pipeName, O_RDONLY);
    FD_SET(fd, &read_fd_set);

    int retval = select(1, &read_fd_set, NULL, NULL);

    switch (retval) {
        case -1:
            printf("Select Error.\n");
            exit(-1);
            break;
        case 1:
            if (FD_ISSET(1), &read_fd_set)
                readCommands();
            break;
        default:

            if (FD_ISSET(fd), &read_fd_set) {
                int pid;
                read
                        char user[MAX_NAME];
                read(fd, user, MAX_NAME);
                checkUsername(user);
            }
            break;
    }
}

int main(int argc, char** argv) {
    char pipeName[PIPE_NAME_MAX];
    createNamedPipe(pipeName, PIPE_SERVER);

    configuraSinal(SIGUSR1);

    checkArgs(argc, argv, &sData);

    getEnvironmentVariables(&eData, &sData);

    initializeMEDITLines(&eData);

    testeSelect(pipeName);
    //readCommands();

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
 * @param sinal
 */
void configuraSinal(int sinal) {
    if (signal(sinal, trataSinal) == SIG_ERR) {
        exitError("Erro a tratar sinal!");
    }

}