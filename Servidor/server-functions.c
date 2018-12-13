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

    strncpy(ed->fileName, "nenhum ficheiro carregado", MAX_FILE_NAME);
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
    for (int i = 0; i < DEFAULT_MAXUSERS; i++) {
        sd->clients[i].valid = 0;
    }
}

/**
 * Função responsável por encontrar a primeira posição disponível no array dos clientes, para que possa ser adicionado um cliente nessa mesma posição.
 * @param sd Estrutura de Dados do Servidor
 * @return posição
 */
int getFirstAvailablePosition(ServerData sd) {
    int j;
    for (j = 0; j < DEFAULT_MAXUSERS && sd.clients[j].valid; j++)
        ;
    return j < DEFAULT_MAXUSERS ? j : -1;

}

/**
 * Função responsável por registar um cliente no array.
 * @param username Nome de Utilizador
 * @param sData Ponteiro para Estrutura de Dados do Servidor
 * @param pos Posição
 * @param fdCli Descritor do Pipe do Cliente
 * @param fdIntPipe Descritor do Pipe Interativo
 */
void registerClient(char* username, ServerData* sData, int pos, int fdCli, int fdIntPipe) {
    sData->clients[pos].valid = 1;
    strncpy(sData->clients[pos].username, username, 9);
    sData->clients[pos].fdPipeClient = fdCli;
    sData->clients[pos].fdIntPipe = fdIntPipe;
}

/**
 * Função responsável por encontrar o pipe com menos clientes atribuídos.
 * @param sd Estrutura de Dados do Servidor
 * @param pipes Array de Pipes Interativos
 * @return posição no array de Pipes
 */
int getIntPipe(ServerData sd, InteractionPipe* pipes) {
    int i, menor = 0;
    for (i = 1; i < sd.numInteractivePipes; i++)
        if (pipes[i].numUsers < pipes[menor].numUsers) {
            menor = i;
        }
    pipes[menor].numUsers++;
    return menor;
}

/**
 * Função responsável por remover um cliente do array de clientes.
 * @param username Nome de Utilizador
 * @param sd Ponteiro para a Estrutura de Dados do Servidor
 */
void removeClient(char* username, ServerData* sd) {
    int i;
    for (i = 0; i < sd->maxUsers; i++) {
        if (sd->clients[i].valid && !strncmp(sd->clients[i].username, username, 9)) {
            sd->clients[i].valid = 0;
            closeNamedPipe(sd->clients[i].fdPipeClient);
            break;
        }
    }
}

/**
 * Função responsável por fechar e apagar os pipes do servidor.
 * @param fdMainPipe Descritor do Pipe Principal
 * @param sd Ponteiro para Estrutura de Dados do Servidor
 * @param pipes Array de Pipes Interativos
 */
void closeAndDeleteServerPipes(int fdMainPipe, ServerData* sd, InteractionPipe* pipes) {
    int i;
    char buffer[] = "close";
    write(fdMainPipe, buffer, strlen(buffer));
    puts("[SERVIDOR] Vou fechar e apagar o pipe principal!");
    closeNamedPipe(fdMainPipe);
    deleteNamedPipe(sd->mainPipe);

    ServerMsg msg;
    msg.code = SERVER_SHUTDOWN;
    for (i = 0; i < sd->maxUsers; i++) {
        if (sd->clients[i].valid == 1) {
            printf("[SERVIDOR] Vou desconectar o cliente %s!\n", sd->clients[i].username);
            write(sd->clients[i].fdPipeClient, &msg, sizeof (msg));
            closeNamedPipe(sd->clients[i].fdPipeClient);
            printf("[SERVIDOR] O cliente %s foi desconectado!\n", sd->clients[i].username);
        }
    }

    puts("[SERVIDOR] Vou fechar e apagar os pipes de interacao!");
    for (i = 0; i < sd->numInteractivePipes; i++) {
        write(pipes[i].fd, buffer, strlen(buffer));
        closeNamedPipe(pipes[i].fd);
        deleteNamedPipe(pipes[i].pipeName);
    }
}