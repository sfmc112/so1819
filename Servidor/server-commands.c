#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "server-utils.h"
#include "server-defaults.h"
#include "server-commands.h"

/**
 * Função responsável por colocar a variável de controlo a 0.
 * @param sd Ponteiro para a Estrutura de Dados do Servidor
 */
void cmdShutdown(ServerData* sd) {
    sd->runServer = 0;
    close(STDIN_FILENO);
}

/**
 * Função responsável por escrever todas as informações atuais do servidor.
 * @param sd Estrutura de Dados do Servidor
 * @param ed Estutura do Editor
 */
void cmdSettings(ServerData sd, EditorData ed) {
    printf("\n\n-----SETTINGS-----\n");
    printf("Numero de linhas do editor: %d\n", ed.lin);
    printf("Numero de colunas do editor: %d\n", ed.col);
    printf("Nome da base de dados de usernames: %s\n", sd.usersDB);
    printf("Numero maximo de utilizadores ativos: %d\n", sd.maxUsers);
    printf("Numero de named pipes de interacao: %d\n", sd.numInteractivePipes);
    printf("Nome do named pipe principal: %s\n", sd.mainPipe);
    printf("Nome do ficheiro em edicao: %s\n", ed.fileName);
    printf("Timeout por inatividade: %d\n\n", ed.timeout);
}

int cmdLoad() {
    puts("Comando load nao implementado!");
    return 0;
}

int cmdSave() {
    puts("Comando save nao implementado!");
    return 0;
}

int cmdFree(char* token) {
    int lineNumber;
    if (sscanf(token, "%d", &lineNumber) != 1) {
        return 1;
    }

    freeLine(lineNumber);

    return 0;
}

int cmdStats() {
    puts("Comando statistics nao implementado!");
    return 0;
}

int cmdUsers() {
    printUsers();
    return 0;
}

int cmdText() {
    printEditor();
    
    return 0;
}

/**
 * Função para verificar se um determinado comando possui um argumento.
 * @param token comando
 * @return 1 se existe argumento, 0 se não.
 */
int checkCommandArgs(char* token) {
    token = strtok(NULL, " ");
    if (token != NULL)
        return 1;
    puts("Falta o segundo parametro");
    return 0;
}

