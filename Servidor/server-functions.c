#include "server-functions.h"
#include "server-users.h"
#include "server-utils.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

/**
 * Função para verificar se existem argumentos de inicialização do programa.
 * @param argc número de argumentos
 * @param argv argumentos
 * @param sd estrutura das configurações principais do servidor
 */
void checkArgs(int argc, char** argv, ServerData* sd) {
    if (argc >= 3 && argc <= 7) {
        char* cmd;
        int res;

        while ((res = getopt(argc, argv, "f:p:n:")) != -1) {
            switch (res) {
                case 'f':
                    cmd = optarg;
                    char temp[FILENAME_MAX];
                    strncpy(temp, cmd, FILENAME_MAX);
                    if (ifFileExists(cmd)) {
                        char* token = strtok(temp, ".");
                        token = strtok(NULL, ".");
                        if (strcmp(token, "db") != 0) {
                            printf("A extensão de ficheiro é inválida.\n");
                        } else
                            strncpy(sd->usersDB, cmd, MAX_SIZE_FILENAME);
                    }
                    break;
                case 'p':
                    cmd = optarg;
                    strncpy(sd->mainPipe, cmd, PIPE_NAME_MAX);
                    break;
                case 'n':
                    cmd = optarg;
                    sscanf(cmd, "%d", &(sd->numInteractivePipes));
                    break;
            }
        }
    }
}

/**
 * Inicializa as estruturas do servidor (variáveis globais)
 * as variáveis de linha/coluna já foram atríbuídas assim como o nome do ficheiro
 * da base de dados dos utilizadores
 */
void resetMEDITLines(EditorData* ed) {
    //Inicializar linhas
    int i, j;

    for (i = 0; i < ed->lin; i++) {
        ed->lines[i].free = 1;
        for (j = 0; j < ed->col; j++)
            ed->lines[i].text[j] = ' ';
    }
}

/**
 * Função que verifica se existem as variáveis de ambiente 
 * MEDIT_MAXLINES e MEDIT_MAXCOLUMNS e vai buscar os seus valores, redefinindo
 * as variáveis presentes na estrutura EditorData caso sejam inferiores aos
 * limites máximos
 * @param ed estrutura de dados com informação do editor de texto
 * @param sd estrutura de dados com informação do servidor
 */
void getEnvironmentVariables(EditorData* ed, ServerData* sd) {
    //Variáveis do Editor

    char *l, *c, *t, *mu;
    int lin, col, timeout, maxusers;

    l = getenv(VAR_AMBIENTE_LINHAS);

    if (l != NULL) {
        lin = atoi(l);
        if (lin < DEFAULT_MAXLINES)
            ed->lin = lin;
        else
            ed->lin = DEFAULT_MAXLINES;
    } else
        ed->lin = DEFAULT_MAXLINES;

    c = getenv(VAR_AMBIENTE_COLUNAS);

    if (c != NULL) {
        col = atoi(c);
        if (col < DEFAULT_MAXCOLUMNS)
            ed->col = col;
        else
            ed->col = DEFAULT_MAXCOLUMNS;
    } else
        ed->col = DEFAULT_MAXCOLUMNS;

    t = getenv(VAR_AMBIENTE_TIMEOUT);

    if (t != NULL) {
        timeout = atoi(t);
        ed->timeout = timeout;
    } else
        ed->timeout = DEFAULT_TIMEOUT;

    //Variáveis Servidor

    mu = getenv(VAR_AMBIENTE_USERS);

    if (mu != NULL) {
        maxusers = atoi(mu);
        if (maxusers < ed->lin)
            sd->maxUsers = maxusers;
        else
            sd->maxUsers = ed->lin;
    } else
        sd->maxUsers = DEFAULT_MAXUSERS;
}

/**
 * Inicializa as estruturas do servidor: base de dados dos users, pipe principal e numero de pipes interativos.
 * @param sd Ponteiro para a estrutura do servidor
 */
void initializeServerData(ServerData* sd) {
    sd->runServer = 1;
    strncpy(sd->mainPipe, MAIN_PIPE_SERVER, PIPE_NAME_MAX);
    strncpy(sd->usersDB, USERSDEFAULT_DB, MAX_SIZE_FILENAME);
    sd->numInteractivePipes = NUM_INTERACTIVE_PIPES;
}