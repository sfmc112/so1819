#include "server-functions.h"
#include "server-users.h"
#include "server-utils.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

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
                            printf("[SERVIDOR] A extensão de ficheiro é inválida.\n");
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
 * Inicializa as estruturas do servidor (variáveis globais). As variáveis de linha/coluna já foram atríbuídas assim como o nome do ficheiro da base de dados dos utilizadores.
 * @param ed ponteiro para estrutura de dados do editor
 */
void resetMEDITLines(EditorData* ed) {
    for (int i = 0; i < ed->lin; i++) {
        ed->lines[i].free = 1;
        strncpy(ed->clients[i], "        ", 8);
        strncpy(ed->authors[i], "        ", 8);
        for (int j = 0; j < ed->col; j++) {
            ed->lines[i].text[j] = ' ';
        }
    }

    strncpy(ed->fileName, "sem titulo", MAX_FILE_NAME);
    ed->numLetters = ed->numWords = 0;

    for (int i = 0; i < 5; i++) {
        ed->mostCommonChars[i] = 0;
    }
}

/**
 * Função que verifica se existem as variáveis de ambiente: MEDIT_MAXLINES e MEDIT_MAXCOLUMNS. Caso existam, vai buscar os seus valores, redefinindo as variáveis presentes na estrutura EditorData, caso as mesmas sejam inferiores aos limites máximos
 * @param ed ponteiro para estrutura de dados com informação do editor de texto
 * @param sd ponteiro para estrutura de dados com informação do servidor
 */
void getEnvironmentVariables(EditorData* ed, ServerData* sd) {
    // Variáveis do Editor
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

    // Variáveis Servidor
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
 * Inicializa as estruturas do servidor: base de dados dos utilizadores, pipe principal e número de pipes interativos.
 * @param sd Ponteiro para a estrutura do servidor
 */
void initializeServerData(ServerData* sd) {
    sd->runServer = 1;
    strncpy(sd->mainPipe, MAIN_PIPE_SERVER, PIPE_NAME_MAX);
    strncpy(sd->usersDB, USERSDEFAULT_DB, MAX_SIZE_FILENAME);
    sd->numInteractivePipes = NUM_INTERACTIVE_PIPES;
    for (int i = 0; i < DEFAULT_MAXUSERS; i++) {
        sd->clients[i].valid = 0;
        sd->clients[i].fdPipeClient = -1;
        sd->clients[i].columnPosition = sd->clients[i].linePosition = 0;
        sd->clients[i].isEditing = 0;
        sd->clients[i].secondsAFK = 0;
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
    strncpy(sData->clients[pos].username, username, 8);
    sData->clients[pos].fdPipeClient = fdCli;
    sData->clients[pos].fdIntPipe = fdIntPipe;
    sData->clients[pos].columnPosition = sData->clients[pos].linePosition = 0;
    sData->clients[pos].isEditing = sData->clients[pos].secondsAFK = sData->clients[pos].secondsSession = 0;
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

/**
 * Função responsável por mover todos os caractéres para a esquerda.
 * @param linha linha de texto
 * @param x posição no array
 * @param max_x numero colunas
 */
void moveAllToTheLeft(char* linha, int x, int max_x) {
    int max = max_x - 1;
    for (; x < max; x++)
        linha[x] = linha[x + 1];
    linha[max] = ' ';
}

/**
 * Função é responsável por mover o texto a partir de uma posição X para a direita.
 * @param linha linha de texto
 * @param x coluna
 * @param max_x valor maximo da coluna (eixo do x)
 * @return 0 se falhou, 1 caso contrário.
 */
int moveAllToTheRight(char* linha, int x, int max_x) {
    int max = max_x - 1;
    if (linha[max] != ' ')
        return 0;
    for (; max > x; max--)
        linha[max] = linha[max - 1];
    return 1;
}

/**
 * Função responsável por encontrar o descritor do pipe do cliente pelo username.
 * @param sd Estrutura de Dados do Servidor
 * @param user nome do utilizador
 * @return descritor do pipe
 */
int getClientPipe(ServerData sd, char* user) {
    for (int i = 0; i < sd.maxUsers; i++)
        if (!strncmp(sd.clients[i].username, user, 8)) {
            return sd.clients[i].fdPipeClient;
        }
    return -1;
}

/**
 * Função responsável por encontrar a posição do cliente no array, atráveis do nome de utilizador.
 * @param sd estrutura de dados do servidor
 * @param user nome do utilizador
 * @return -1 caso não exista, posição no array de clientes caso contrário.
 */
int getClientArrayPosition(ServerData sd, char* user) {
    for (int i = 0; i < sd.maxUsers; i++)
        if (!strncmp(sd.clients[i].username, user, 8)) {
            return i;
        }
    return -1;
}

/**
 * Função responsável por devolver a percentagem de linhas que foram editadas por último pelo cliente enviado por argumento.
 * @param user nome de utilizador
 * @param ed estrutura de dados do editor
 * @return percentagem
 */
int getPercentage(char* user, EditorData ed) {
    int count = 0;

    int i;
    for (i = 0; i < ed.lin; i++) {
        if (!strncmp(user, ed.authors[i], 8))
            count++;
    }

    return count * 100 / ed.lin;
}

/**
 * Função responsável por verificar se um determinado cliente já existe no array
 * @param users array de indexs de clientes
 * @param index index do cliente
 * @return 1 caso exista, 0 caso contrário
 */
int ifExists(int* users, int index) {
    int i = 0;
    while (users[i] != -1) {
        if (users[i] == index) return 1;
        i++;
    }
    return 0;
}

/**
 * Função responsável por obter o cliente com maior tempo de sessão, sendo que o mesmo não pode jã́já ter sido escolhido anteriormente.
 * @param users array de indexs de utilizadores
 * @param clients ponteiro para estrutura de dados dos clientes
 * @param size quantidade de clientes
 * @return index do cliente com maior sessão e não repetido.
 */
int getMax(int* users, ClientData* clients, int size) {
    int max = -1;
    int i;
    for (i = 0; i < size; i++) {
        if (clients[i].valid) {
            if (clients[i].secondsSession > max && !ifExists(users, i))
                max = clients[i].secondsSession;
        }
    }

    for (i = 0; i < size; i++) {
        if (clients[i].valid && clients[i].secondsSession == max && !ifExists(users, i))
            return i;
    }

    return -1;
}

/**
 * Função responsável por colocar os utilizadores ordenados por tempo de sessão.
 * @param users array de indexs de utilizadores
 * @param clients ponteiro para estrutura de dados dos clientes
 * @param size quantidade de clientes
 */
void getUsersOrderedBySessionDuration(int* users, ClientData* clients, int size) {
    int i = 0, sair;
    users[i] = -1;

    do {
        sair = getMax(users, clients, size);
        if (sair != -1) {
            users[i] = sair;
            i++;
            users[i] = -1;
        }
    } while (sair != -1);
}

/**
 * Função responsável por enviar mensagem para todos os clientes com o código EDITOR_UPDATE, fazendo com que os mesmos atualizem o editor.
 * @param ed estrutura de dados do editor
 * @param sd estrutura de dados do servidor
 */
void sendMessageEditorUpdateToAllClients(EditorData ed, ServerData sd) {
    ServerMsg smsg;
    smsg.code = EDITOR_UPDATE;
    smsg.ed = ed;

    writeToAllClients(sd, smsg);
}

/**
 * Função reponsável por enviar uma mensagem para um determinado cliente.
 * @param c estrutura de dados do cliente
 * @param smsg estrutura de mensagem do servidor
 */
void writeToAClient(ClientData c, ServerMsg smsg) {
    //printf("A enviar msg tipo %d no descritor %d\n", smsg.code, c.fdPipeClient);
    smsg.cursorLinePosition = c.linePosition;
    smsg.cursorColumnPosition = c.columnPosition;
    write(c.fdPipeClient, &smsg, sizeof (smsg));
}

/**
 * Função responsável por enviar mensagem para todos os clientes
 * @param sd estrutura de dados do servidor
 * @param smsg estrutura de mensagem do servidor
 */
void writeToAllClients(ServerData sd, ServerMsg smsg) {
    for (int i = 0; i < sd.maxUsers; i++) {
        if (sd.clients[i].valid) {
            writeToAClient(sd.clients[i], smsg);
        }
    }
}

/**
 * Função responsável por contar as palavras no editor.
 * @param eData estrutura de dados do editor
 * @return quantidade de palavras presentes no texto do editor
 */
int countNumberOfWords(EditorData eData) {
    int count = 0;
    char linha[eData.col + 1];

    for (int i = 0; i < eData.lin; i++) {
        strncpy(linha, eData.lines[i].text, eData.col);
        linha[eData.col] = 0;
        count += contaPalavras(linha);
    }
    return count;
}

/**
 * Função responsável por contar as letras do editor.
 * @param eData estrutura de dados do editor
 * @return quantidade de letras
 */
int countNumberofLetters(EditorData eData) {
    int count = 0;

    for (int i = 0; i < eData.lin; i++) {
        for (int j = 0; j < eData.col; j++) {
            if (isalpha(eData.lines[i].text[j]))
                count++;
        }
    }
    return count;
}

/**
 * Função responsável por descobrir quais os caractéres mais utilizados no momento.
 * @param mostCommonChars array de caractéres
 * @param eData estrutura de dados do editor
 */
void getMostCommonChars(char* mostCommonChars, EditorData eData) {
    for (int i = 0; i < 5; i++)
        mostCommonChars[i] = 0;

    int numUniqueChars = 0;
    char* uniqueChars;
    uniqueChars = getArrayOfUniqueChars(eData, &numUniqueChars);

    if (uniqueChars == NULL) {
        //errorMessage("Nao ha caracteres diferentes ou o documento esta vazio");
        return;
    }

    int numberOfChars[numUniqueChars];

    for (int i = 0; i < numUniqueChars; i++) {
        numberOfChars[i] = countChars(eData, uniqueChars[i]);
    }

    // Procurar os 5 maiores
    int count = 0;
    int max = -1;
    int index = -1;
    int i;

    while (count < 5) {
        max = index = -1;
        for (int i = 0; i < numUniqueChars; i++) {
            if (max < numberOfChars[i] && !doesCharExistInArray(mostCommonChars, 5, uniqueChars[i])) {
                max = numberOfChars[i];
                index = i;
            }
        }
        if (index == -1)
            break;
        mostCommonChars[count++] = uniqueChars[index];
    }
}