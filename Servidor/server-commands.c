#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "server-utils.h"
#include "server-defaults.h"
#include "server-commands.h"
#include "server-functions.h"

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

/**
 * Função responsável por verificar se existe algum ficheiro com o nome enviado por argumento. Caso exista, esse ficheiro será carregado, descartando o atual.
 * @param token comando completo
 */
void cmdLoad(char* token) {
    char nomeFicheiro[MAX_FILE_NAME];
    sscanf(token, "%s", nomeFicheiro);
    if (ifFileExists(nomeFicheiro))
        loadDocument(nomeFicheiro);
}

/**
 * Função responsável por guardar o texto atual do editor num ficheiro com o nome enviado por argumento.
 * @param token comando completo
 */
void cmdSave(char* token) {
    char nomeFicheiro[MAX_FILE_NAME];
    sscanf(token, "%s", nomeFicheiro);
    //puts("Vou gravar o ficheiro");
    saveDocument(nomeFicheiro);
}

/**
 * Função responsável por libertar uma linha (colocar linha disponível para edição, descartando alterações).
 * @param token comando completo
 */
void cmdFree(char* token) {
    int lineNumber;
    if (sscanf(token, "%d", &lineNumber) == 1) {
        printf("\n\n-----FREE-----\n");
        freeOneLine(lineNumber);
    }
}

/**
 * Função responsável por colocar a variável de controlo de listagem de informação de estatísticas a 1.
 * @param print variável de controlo de listagem de informação de estatísticas
 */
void cmdStats(int* print) {
    *print = 1;
}

/**
 * Função responsável por listar utilizadores.
 */
void cmdUsers() {
    printUsers();
}

/**
 * Função responsável por mostrar o editor.
 */
void cmdText() {
    printEditor();
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
    puts("\n[SERVIDOR] Falta o segundo parametro");
    return 0;
}