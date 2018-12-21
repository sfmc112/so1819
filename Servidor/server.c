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
void createServerStartingThreads(pthread_t* timeouts, pthread_t* mainpipe, pthread_t intpipes[], InteractionPipe* pipes);
void* readFromMainPipe(void* arg);
void* readFromIntPipe(void* arg);
void joinThreads(pthread_t mainpipe, pthread_t intpipes[]);
void testaAspell();
void writeToAClient(ClientData c, ServerMsg smsg);
void writeToAllClients(ServerData sd, ServerMsg smsg);
void* threadCheckTimeouts();

EditorData eData;
ServerData sData;
int fdToAspell = -1;
int fdFromAspell = -1;
int fdMainPipe = -1;

// Mutexes
//pthread_mutex_t mutexAspell;
pthread_mutex_t mutexClientData;
//pthread_mutex_t mutexEditorData;

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

    //testaAspell();

    // Inicializar mutexes
    //mutexAspell = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&mutexClientData, NULL);
    //mutexEditorData = PTHREAD_MUTEX_INITIALIZER;

    //Inicializar threads;
    pthread_t idCheckTimeout;
    pthread_t idMainPipe;
    pthread_t idIntPipes[sData.numInteractivePipes];

    createServerStartingThreads(&idCheckTimeout, &idMainPipe, idIntPipes, interactivePipes);

    readCommands();
    
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
void* readCommands() {
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
void createServerStartingThreads(pthread_t* timeouts, pthread_t* mainpipe, pthread_t intpipes[], InteractionPipe * pipes) {
    puts("[SERVIDOR] Vao ser criadas as threads!");

    int err;
    
    err = pthread_create(timeouts, NULL, threadCheckTimeouts, NULL);
    if (err)
        printf("[SERVIDOR] Nao foi possível criar a thread :[%s]\n", strerror(err));
    else
        printf("[SERVIDOR] A thread responsavel por ler o pipe principal foi criada!\n");

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
            printf("\nDescritor do cliente %s: %d\n\n", login.username, fdCli);
            if (fdCli == -1)
                continue;
            pthread_mutex_lock(&mutexClientData);
            pos = getFirstAvailablePosition(sData);
            //printf("Pos: %d\n", pos);
            if (!checkUsername(login.username) || checkUserOnline(login.username, sData) || pos == -1) {
                msg.code = LOGIN_FAILURE;
                printf("[SERVIDOR] Falhou o login!\n");
            } else {
                msg.code = LOGIN_SUCCESS;
                msg.ed = eData;
                int index = getIntPipe(sData, pipes);
                strncpy(msg.intPipeName, pipes[index].pipeName, PIPE_MAX_NAME);
                registerClient(login.username, &sData, pos, fdCli, pipes[index].fd);
                printf("[SERVIDOR] O utilizador %s conectou-se!\n", login.username);
                printf("Descritor atual %d\n", sData.clients[getClientArrayPosition(sData, login.username)].fdPipeClient);
            }
            pthread_mutex_unlock(&mutexClientData);
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
    ServerMsg smsg;
    int nBytes;

    InteractionPipe* pipeI = (InteractionPipe*) arg;
    while (sData.runServer) {
        nBytes = read(pipeI->fd, &msg, sizeof (msg));
        pthread_mutex_lock(&mutexClientData);
        smsg.code = EDITOR_ERROR;
        if (nBytes == sizeof (msg)) {
            printf("Msg tipo %d\n", msg.msgType);
            printf("Cliente %s\nLetra %c\n", msg.username, msg.letra);

            int indexClient = getClientArrayPosition(sData, msg.username);
            int yPos = sData.clients[indexClient].linePosition;
            int xPos = sData.clients[indexClient].columnPosition;
            int state = sData.clients[indexClient].isEditing; // 0 - Navegação | 1 - Edição
            
            // Fazer reset ao contador do timeout
            sData.clients[indexClient].secondsAFK = 0;
            
            switch (msg.msgType) {
                case CLIENT_SHUTDOWN:
                    removeClient(msg.username, &sData);
                    printf("[SERVIDOR] O utilizador %s desconectou-se!\n", msg.username);
                    pipeI->numUsers--;
                    break;
                case K_ENTER:
                    if (!state) {
                        if (eData.lines[yPos].free == 1) {
                            strncpy(eData.clients[yPos], msg.username, 9);
                            eData.lines[yPos].free = 0;
                            strncpy(sData.clients[indexClient].oldText, eData.lines[yPos].text, eData.col);
                            smsg.code = EDITOR_START;
                            state = !state;
                        }
                    } else {
                        puts("Vou perguntar ao Aspell se isto esta bem");
                        printf("A frase e %s\n", eData.lines[yPos].text);

                        char temp[DEFAULT_MAXCOLUMNS];
                        strncpy(temp, eData.lines[yPos].text, DEFAULT_MAXCOLUMNS - 1);
                        temp[DEFAULT_MAXCOLUMNS - 1] = '\0';

                        if (spellCheckSentence(temp, fdToAspell, fdFromAspell) == 0) {
                            puts("Vou sair do modo de edicao porque a frase esta correta");
                            state = !state;
                            strncpy(eData.clients[yPos], "        ", 8);
                            eData.lines[yPos].free = 1;
                            smsg.code = EDITOR_UPDATE;
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
        pthread_mutex_unlock(&mutexClientData);
    }
    return NULL;
}

void writeToAClient(ClientData c, ServerMsg smsg) {
    printf("A enviar msg tipo %d no descritor %d\n", smsg.code, c.fdPipeClient);
    smsg.cursorLinePosition = c.linePosition;
    smsg.cursorColumnPosition = c.columnPosition;
    write(c.fdPipeClient, &smsg, sizeof (smsg));
}

void writeToAllClients(ServerData sd, ServerMsg smsg) {
    int i;
    for (i = 0; i < sd.maxUsers; i++) {
        if (sd.clients[i].valid) {
            writeToAClient(sd.clients[i], smsg);
        }
    }
}

void* threadCheckTimeouts() {
    int i;

    while (sData.runServer) {
        for (i = 0; i < sData.maxUsers; i++) {
            if (sData.clients[i].valid && sData.clients[i].isEditing){
                pthread_mutex_lock(&mutexClientData);
                sData.clients[i].secondsAFK++;
                pthread_mutex_unlock(&mutexClientData);
            }
        }
        sleep(1);
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
    puts("[ASPELL] A verificar a frase 'O Gabriel limpa a cozinha'...");
    if (spellCheckSentence("O Gabriel limpa a cozinha", fdToAspell, fdFromAspell) == 0) {
        puts("[ASPELL] Esta correto");
    } else {
        puts("[ASPELL] Esta incorreto");
    }
}