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

void readCommands();
void trataSinal(int numSinal);
void configuraSinal(int sinal);
void createNamedPipesServer(InteractionPipe* pipes);
void openNamedPipesServer(InteractionPipe* pipes);
void initializeInteractivePipes(InteractionPipe* pipes);
void createServerStartingThreads(pthread_t* timeouts, pthread_t* mainpipe, pthread_t intpipes[], InteractionPipe* pipes);
void* readFromMainPipe(void* arg);
void* readFromIntPipe(void* arg);
void joinThreads(pthread_t mainpipe, pthread_t intpipes[]);
void writeToAClient(ClientData c, ServerMsg smsg);
void writeToAllClients(ServerData sd, ServerMsg smsg);
void* threadCheckTimeouts();

EditorData eData;
ServerData sData;
int fdToAspell = -1;
int fdFromAspell = -1;
int fdMainPipe = -1;

// Mutexes
pthread_mutex_t mutexData;

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

    // Inicializar mutexes
    pthread_mutex_init(&mutexData, NULL);

    // Inicializar threads;
    pthread_t idCheckTimeout;
    pthread_t idMainPipe;
    pthread_t idIntPipes[sData.numInteractivePipes];

    createServerStartingThreads(&idCheckTimeout, &idMainPipe, idIntPipes, interactivePipes);

    readCommands();

    // Fechar o programa
    puts("[SERVIDOR] Vai ser terminado.");

    closeAndDeleteServerPipes(fdMainPipe, &sData, interactivePipes);

    puts("[ASPELL] Vai ser terminado.");
    close(fdToAspell); // Vai fazer terminar o Aspell

    joinThreads(idMainPipe, idIntPipes);

    remove("/tmp/unique.txt");
    return (EXIT_SUCCESS);
}

/**
 * Função responsável por ler o comando e interpreta-o.
 */
void readCommands() {
    char comando[MAX_INPUT];
    char temp[MAX_INPUT];
    int printStats = 0;
    const char* listaComandos[] = {"shutdown", "settings", "load", "save", "free", "statistics", "users", "text"};
    char* token = NULL;
    setbuf(stdout, NULL);
    int i, err;

    // Thread para calcular e imprimir estatísticas
    pthread_t stats;
    err = pthread_create(&stats, NULL, editorStats, (void*) &printStats);

    if (err)
        printf("[SERVIDOR] Nao foi possível criar a thread :[%s]\n", strerror(err));
    else
        printf("[SERVIDOR] A thread responsavel por calcular estatisticas foi criada!\n");

    // Ciclo principal
    while (sData.runServer) {
        printf("\n[SERVIDOR] Introduza o comando: ");
        scanf(" %39[^\n]", comando);
        printStats = 0;
        // comando tudo em letras minusculas
        toLower(comando);
        strncpy(temp, comando, MAX_INPUT);
        //1ª parte do comando
        token = strtok(comando, " ");
        // Imprime comando
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
                if (checkCommandArgs(token)) {
                    token = strtok(temp, " ");
                    token = strtok(NULL, " ");
                    cmdLoad(token);
                }
                break;
            case 3:
                if (checkCommandArgs(token)) {
                    token = strtok(temp, " ");
                    token = strtok(NULL, " ");
                    cmdSave(token);
                }
                break;
            case 4:
                if (checkCommandArgs(token)) {
                    token = strtok(temp, " ");
                    token = strtok(NULL, " ");
                    cmdFree(token);
                }
                break;
            case 5:
                cmdStats(&printStats);
                break;
            case 6:
                cmdUsers();
                break;
            case 7:
                cmdText();
                break;
            default:
                puts("\n[SERVIDOR] Comando invalido!\n");
        }
    }
}

/**
 * Função responsável por executar o comportamento de numSinal.
 * @param numSinal Código do sinal.
 */
void trataSinal(int numSinal) {
    if (numSinal == SIGUSR1 || numSinal == SIGINT) {
        //puts("Recebi sinal");
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
    puts("[SERVIDOR] A inicializar a informacao relativa aos pipes de interacao...");
    for (int i = 0; i < sData.numInteractivePipes; i++) {
        pipes[i].numUsers = 0;
    }
}

/**
 * Função responsável por criar as threads.
 * @param timeouts Thread para contabilizar timeouts
 * @param mainpipe Thread para ler do pipe principal do servidor
 * @param intpipes Array de Threads para os pipes interativos
 * @param pipes Array dos Pipes Interativos
 */
void createServerStartingThreads(pthread_t* timeouts, pthread_t* mainpipe, pthread_t intpipes[], InteractionPipe * pipes) {
    puts("[SERVIDOR] Vao ser criadas as threads!");

    int err;

    err = pthread_create(timeouts, NULL, threadCheckTimeouts, NULL);
    if (err)
        printf("[SERVIDOR] Nao foi possível criar a thread :[%s]\n", strerror(err));
    else
        printf("[SERVIDOR] A thread responsavel por contabilizar timeouts foi criada!\n");

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
 * @param arg Array de pipes interativos
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
            fdCli = openNamedPipe(login.nomePipeCliente, O_WRONLY);
            if (fdCli == -1)
                continue;
            pthread_mutex_lock(&mutexData);
            pos = getFirstAvailablePosition(sData);
            //printf("Pos: %d\n", pos);
            if (!checkUsername(login.username) || checkUserOnline(login.username, sData) || pos == -1) {
                msg.code = LOGIN_FAILURE;
                printf("\n[SERVIDOR] Falhou o login!\n");
            } else {
                msg.code = LOGIN_SUCCESS;
                msg.ed = eData;
                int index = getIntPipe(sData, pipes);
                strncpy(msg.intPipeName, pipes[index].pipeName, PIPE_MAX_NAME);
                registerClient(login.username, &sData, pos, fdCli, pipes[index].fd);
                strncpy(sData.clients[pos].nameIntPipe, pipes[index].pipeName, PIPE_MAX_NAME);
                printf("\n[SERVIDOR] O utilizador %s conectou-se!\n", login.username);
            }
            pthread_mutex_unlock(&mutexData);
            write(fdCli, &msg, sizeof (msg));
        }
    }
    return NULL;
}

/**
 * Função repsonsável por efetuar a leitura dos pipes interativos. 
 * @param arg array de pipes interativos
 * @return Ponteiro para void (void*)
 */
void* readFromIntPipe(void* arg) {
    ClientMsg msg;
    ServerMsg smsg;
    int nBytes;

    InteractionPipe* pipeI = (InteractionPipe*) arg;
    while (sData.runServer) {
        nBytes = read(pipeI->fd, &msg, sizeof (msg));
        pthread_mutex_lock(&mutexData);
        smsg.code = EDITOR_ERROR;
        if (nBytes == sizeof (msg)) {
            //printf("Msg tipo %d\n", msg.msgType);
            //printf("Cliente %s\nLetra %c\n", msg.username, msg.letra);

            int indexClient = getClientArrayPosition(sData, msg.username);
            int yPos = sData.clients[indexClient].linePosition;
            int xPos = sData.clients[indexClient].columnPosition;
            int state = sData.clients[indexClient].isEditing; // 0 - Navegação | 1 - Edição

            // Fazer reset ao contador do timeout
            sData.clients[indexClient].secondsAFK = 0;

            switch (msg.msgType) {
                case CLIENT_SHUTDOWN:
                    removeClient(msg.username, &sData);
                    printf("\n[SERVIDOR] O utilizador %s desconectou-se!\n", msg.username);
                    pipeI->numUsers--;
                    break;
                case K_ENTER:
                    if (!state) {
                        if (eData.lines[yPos].free == 1) {
                            strncpy(eData.clients[yPos], msg.username, 8);
                            eData.lines[yPos].free = 0;
                            strncpy(sData.clients[indexClient].oldText, eData.lines[yPos].text, eData.col);
                            smsg.code = EDITOR_START;
                            state = !state;
                        }
                    } else {
                        //puts("Vou perguntar ao Aspell se isto esta bem");
                        //printf("A frase e %s\n", eData.lines[yPos].text);

                        char temp[DEFAULT_MAXCOLUMNS];
                        strncpy(temp, eData.lines[yPos].text, DEFAULT_MAXCOLUMNS - 1);
                        temp[DEFAULT_MAXCOLUMNS - 1] = '\0';

                        if (spellCheckSentence(temp, fdToAspell, fdFromAspell) == 0) {
                            //puts("Vou sair do modo de edicao porque a frase esta correta");
                            state = !state;
                            if (strncmp(eData.lines[yPos].text, sData.clients[indexClient].oldText, eData.col))
                                strncpy(eData.authors[yPos], eData.clients[yPos], 8);
                            strncpy(eData.clients[yPos], "        ", 8);
                            eData.lines[yPos].free = 1;
                            smsg.code = EDITOR_UPDATE;
                        } else {
                            smsg.code = ASPELL_ERROR;
                        }
                    }
                    break;
                case K_ESC:
                    if (!state) {
                        smsg.code = EDITOR_SHUTDOWN;
                    } else {
                        state = !state;
                        strncpy(eData.clients[yPos], "        ", 8);
                        strncpy(eData.lines[yPos].text, sData.clients[indexClient].oldText, eData.col);
                        eData.lines[yPos].free = 1;
                        xPos = 0;
                        smsg.code = EDITOR_UPDATE;
                    }
                    break;
                case K_BACKSPACE:
                    if (state) {
                        if (xPos > 0) {
                            moveAllToTheLeft(eData.lines[yPos].text, xPos - 1, eData.col);
                            xPos--;
                            smsg.code = EDITOR_UPDATE;
                        }
                    }
                    break;
                case K_DEL:
                    if (state) {
                        moveAllToTheLeft(eData.lines[yPos].text, xPos, eData.col);
                        smsg.code = EDITOR_UPDATE;
                    }
                    break;
                case K_CHAR:
                    if (state) {
                        if (moveAllToTheRight(eData.lines[yPos].text, xPos, eData.col)) {
                            eData.lines[yPos].text[xPos] = msg.letra;
                            if (xPos < eData.col - 1)
                                xPos++;
                            smsg.code = EDITOR_UPDATE;
                        }
                    }
                    break;
                case MOVE_UP:
                    if (!state) {
                        if (yPos > 0) {
                            yPos--;
                            smsg.code = EDITOR_UPDATE;
                        }
                    }
                    break;
                case MOVE_DOWN:
                    if (!state) {
                        if (yPos < eData.lin - 1) {
                            yPos++;
                            smsg.code = EDITOR_UPDATE;
                        }
                    }
                    break;
                case MOVE_LEFT:
                    if (xPos > 0) {
                        xPos--;
                        smsg.code = EDITOR_UPDATE;
                    }
                    break;
                case MOVE_RIGHT:
                    if (xPos < eData.col - 1) {
                        xPos++;
                        smsg.code = EDITOR_UPDATE;
                    }
                    break;
                default:
                    break;
            }
            smsg.ed = eData;
            sData.clients[indexClient].linePosition = yPos;
            sData.clients[indexClient].columnPosition = xPos;
            sData.clients[indexClient].isEditing = state;

            if (smsg.code == EDITOR_UPDATE || smsg.code == EDITOR_START)
                writeToAllClients(sData, smsg);

            else
                writeToAClient(sData.clients[indexClient], smsg);
        }
        pthread_mutex_unlock(&mutexData);
    }
    return NULL;
}

/**
 * Função responsável por contabilizar os timeouts.
 * @return NULL
 */
void* threadCheckTimeouts() {
    int i;
    ServerMsg smsg;

    while (sData.runServer) {
        for (i = 0; i < sData.maxUsers; i++) {
            sData.clients[i].secondsSession++;
            if (sData.clients[i].valid && sData.clients[i].isEditing) {
                pthread_mutex_lock(&mutexData);
                sData.clients[i].secondsAFK++;
                if (sData.clients[i].secondsAFK >= eData.timeout) {
                    printf("\n[SERVIDOR] O cliente %s ficou inativo.\n", sData.clients[i].username);
                    freeLine(sData.clients[i].linePosition);
                    smsg.code = TIMEOUT;
                    smsg.ed = eData;
                    smsg.cursorLinePosition = sData.clients[i].linePosition;
                    smsg.cursorColumnPosition = sData.clients[i].columnPosition;
                    writeToAClient(sData.clients[i], smsg);
                    sendMessageEditorUpdateToAllClients(eData, sData);
                }
                pthread_mutex_unlock(&mutexData);
            }
        }
        sleep(1);
    }
    return NULL;
}

/**
 * Função responsável por libertar uma determinada linha.
 * @param lineNumber index da linha
 */
void freeLine(int lineNumber) {
    if (lineNumber >= 0 && lineNumber < eData.lin && eData.lines[lineNumber].free == 0) {
        printf("\n[SERVIDOR] Vou libertar a linha %d.\n", lineNumber);
        // Ir buscar o indíce do cliente no array de ClientData
        int index = getClientArrayPosition(sData, eData.clients[lineNumber]);
        strncpy(eData.clients[lineNumber], "        ", 8);
        strncpy(eData.lines[lineNumber].text, sData.clients[index].oldText, eData.col);
        eData.lines[lineNumber].free = 1;
        sData.clients[index].isEditing = 0;
    }
}

/**
 * Função responsável por mostrar o editor completo.
 */
void printEditor() {
    //char ecra[eData.lin][12 + eData.col];

    pthread_mutex_lock(&mutexData);
    printf("\n\n-----TEXT-----\n");
    for (int i = 0; i < eData.lin; i++) {
        //snprintf(ecra[i], 12 + eData.col, "%s %02d %s", eData.clients[i], i, eData.lines[i].text);
        //printf("%s\n", ecra[i]);
        printf("%-8s %02d %s\n", eData.clients[i], i, eData.lines[i].text);
    }
    pthread_mutex_unlock(&mutexData);
    putchar('\n');
}

/**
 * Função responsável por listar os utilizadores ordenados por tempo de sessão.
 */
void printUsers() {
    int userIndex[sData.maxUsers];

    getUsersOrderedBySessionDuration(userIndex, sData.clients, sData.maxUsers);

    int i;

    printf("\n\n-----USERS-----\n");
    for (i = 0; i < sData.maxUsers && userIndex[i] != -1; i++) {
        printf("User: %s\nIdade Sessao: %4d segundos\nNome Pipe: %s\nPercentagem de linhas da sua autoria: %d%%",
                sData.clients[userIndex[i]].username, sData.clients[userIndex[i]].secondsSession, sData.clients[userIndex[i]].nameIntPipe, getPercentage(sData.clients[userIndex[i]].username, eData));
        if (sData.clients[userIndex[i]].isEditing)
            printf("\nLinha em edicao %02d.", sData.clients[userIndex[i]].linePosition);
        printf("\n\n");
    }
}

/**
 * Função responsável por carregar o texto do ficheiro enviado por argumento, descartando o atual.
 * @param nomeFicheiro nome do ficheiro
 */
void loadDocument(char* nomeFicheiro) {
    FILE *file;

    file = fopen(nomeFicheiro, "rt");
    if (file == NULL) {
        errorMessage("Houve um erro a abrir o ficheiro.");
        return;
    }

    char editorTemp[eData.lin][eData.col];
    char textTemp[eData.col + 1];
    int i = 0;
    char * pointer;


    while ((pointer = fgets(textTemp, eData.col + 1, file)) != NULL && i < eData.lin) {
        //puts("DEBUG 1");
        int ind = 0;
        while (ind < eData.col && textTemp[ind] != '\n')
            ind++;
        if (ind < eData.col) {
            for (; ind < eData.col; ind++)
                textTemp[ind] = ' ';
        } else
            fgetc(file);

        if (isLineEmpty(textTemp, eData.col)) {
            //puts("DEBUG 2");
            for (int j = 0; j < eData.col; j++) {
                editorTemp[i][j] = ' ';
            }
        } else if (!spellCheckSentence(textTemp, fdToAspell, fdFromAspell)) {
            //puts("DEBUG 3");
            for (int j = 0; j < eData.col; j++) {
                editorTemp[i][j] = textTemp[j];
            }
        } else {
            //puts("DEBUG 4");
            for (int j = 0; j < eData.col; j++) {
                editorTemp[i][j] = ' ';
            }
        }
        i++;
    }

    fclose(file);

    // Colocar as restantes linhas nao lidas em branco
    for (int j = i; j < eData.lin; j++) {
        for (int k = 0; k < eData.col; k++) {
            editorTemp[j][k] = ' ';
        }
    }
    printf("\n\n-----LOAD-----\n");
    printf("\n[SERVIDOR] Documento \"%s\" carregado.\n", nomeFicheiro);

    // Alterar o texto original com este, libertando as linhas em edicao e notificar os clientes.
    pthread_mutex_lock(&mutexData);
    for (i = 0; i < eData.lin; i++) {
        freeLine(i);
    }
    for (i = 0; i < eData.lin; i++) {
        for (int j = 0; j < eData.col; j++) {
            eData.lines[i].text[j] = editorTemp[i][j];
        }
    }
    strncpy(eData.fileName, nomeFicheiro, MAX_FILE_NAME);
    sendMessageEditorUpdateToAllClients(eData, sData);
    pthread_mutex_unlock(&mutexData);
}

/**
 * Função responsável por guardar o texto atual do editor num ficheiro com o nome que foi enviado por argumento.
 * @param nomeFicheiro nome do ficheiro
 */
void saveDocument(char* nomeFicheiro) {
    char editorTemp[eData.lin][eData.col];
    int i = 0, j = 0;

    pthread_mutex_lock(&mutexData);
    for (i = 0; i < eData.lin; i++) {
        freeLine(i);
    }

    //puts("Vou fazer spellcheck");

    for (i = 0; i < eData.lin; i++) {
        //puts("DEBUG 1");

        if (!isLineEmpty(eData.lines[i].text, eData.col) && !spellCheckSentence(eData.lines[i].text, fdToAspell, fdFromAspell)) {
            //puts("DEBUG 2");
            for (j = 0; j < eData.col; j++) {
                editorTemp[i][j] = eData.lines[i].text[j];
            }
        } else {
            //puts("DEBUG 3");
            for (int k = 0; k < eData.col; k++) {
                editorTemp[i][k] = ' ';
            }
        }
    }
    sendMessageEditorUpdateToAllClients(eData, sData);
    pthread_mutex_unlock(&mutexData);

    FILE *f;
    f = fopen(nomeFicheiro, "wt");

    if (f != NULL) {
        for (int i = 0; i < eData.lin; i++) {
            for (int j = 0; j < eData.col; j++) {
                fprintf(f, "%c", editorTemp[i][j]);
            }
            fputc('\n', f);
        }
        fclose(f);
        printf("\n\n-----SAVE-----\n");
        printf("\n[SERVIDOR]: Gravei o ficheiro %s\n", nomeFicheiro);
    }
}

/**
 * Função responsável por atualizar sempre as estatísticas do editor.
 * @param param variável de controlo para imprimir no ecrã estas estatísticas.
 * @return NULL
 */
void* editorStats(void* param) {
    int* print = (int*) param;
    int numWords, numLetters;
    char mostCommonChars[5];


    while (sData.runServer) {
        // Número total de palavras
        numWords = countNumberOfWords(eData);
        // Número de letras
        numLetters = countNumberofLetters(eData);
        // 5 caracteres mais comuns
        getMostCommonChars(mostCommonChars, eData);

        if (*print) {
            printf("\n\n-----STATISTICS-----\n");
            printf("Numero total de palavras: %d\n", numWords);
            printf("Numero total de letras: %d\n", numLetters);
            printf("Caracteres mais comuns: ");

            for (int i = 0; i < 5; i++) {
                if (mostCommonChars[i] == '\0')
                    break;
                printf("%c\t", mostCommonChars[i]);
            }
            printf("\n\n");
        }

        pthread_mutex_lock(&mutexData);

        eData.numWords = numWords;
        eData.numLetters = numLetters;

        for (int i = 0; i < 5; i++) {
            eData.mostCommonChars[i] = mostCommonChars[i];
        }

        ServerMsg smsg;
        smsg.code = STATS_UPDATE;
        smsg.ed = eData;
        writeToAllClients(sData, smsg);

        pthread_mutex_unlock(&mutexData);

        sleep(1);
    }
    return NULL;
}

/**
 * Função responsável por libertar uma determinada linha.
 * @param lineNumber index da linha
 */
void freeOneLine(int lineNumber) {
    pthread_mutex_lock(&mutexData);
    freeLine(lineNumber);
    sendMessageEditorUpdateToAllClients(eData, sData);
    pthread_mutex_unlock(&mutexData);
}