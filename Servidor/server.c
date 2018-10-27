#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "server-commands.h"
#include "server-functions.h"
#include "server-utils.h"

int readCommands();
void trataSinal(int numSinal);
void configuraSinal(int sinal);

EditorData eData;
ServerData sData;

int main(int argc, char** argv) {

    configuraSinal(SIGUSR1);

    checkArgs(argc, argv, &sData);

    getEnvironmentVariables(&eData, &sData);

    initializeMEDITLines(&eData);

    readCommands();

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

void trataSinal(int numSinal) {
    if (numSinal == SIGUSR1) {
        exitNormal();
    }
}

void configuraSinal(int sinal) {
    if (signal(sinal, trataSinal) == SIG_ERR) {
        exitError("Erro a tratar sinal!");
    }
}