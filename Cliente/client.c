#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "biblioteca.h"
#include "client-functions.h"
#include <pthread.h>
#include <signal.h>
#include <ncurses.h>

void sendLoginToServer(char* user);
void exitClient();
void exitLoginFailure();
void createClientStartingThreads(pthread_t* idEditor, pthread_t* idMyPipe);
void* startEditor();
void* readFromMyPipe();
void configureSignalBeforeLogin(int sinal);
void configureSignalAfterLogin(int sinal);
void exitServerShutdown();

int runClient = 1;
int fdMyPipe = -1, fdSv = -1;
char user[9];
char myPipe[PIPE_NAME_MAX];
char servPipe[PIPE_NAME_MAX];
EditorData ed;
pthread_t idEditor;
pthread_t idMyPipe;

int main(int argc, char** argv) {
    configureSignalBeforeLogin(SIGINT);
    configureSignalBeforeLogin(SIGPIPE);

    char tempPipe[PIPE_NAME_MAX];

    strncpy(servPipe, MAIN_PIPE_SERVER, PIPE_NAME_MAX);
    strncpy(myPipe, PIPE_USER, PIPE_NAME_MAX);
    puts("[CLIENTE] Vou criar o pipe principal!");
    createNamedPipe(tempPipe, myPipe);
    strncpy(myPipe, tempPipe, PIPE_NAME_MAX);

    int flag = checkArgs(argc, argv, servPipe, user);
    puts("[CLIENTE] Vou abrir o pipe principal!");
    fdSv = openNamedPipe(servPipe, O_WRONLY);

    if (fdSv == -1) {
        fprintf(stderr, "[ERRO]: O pipe principal do servidor, nao esta disponivel!\n");
        return EXIT_FAILURE;
    }

    if (flag)
        loginSession(user);

    sendLoginToServer(user);

    configureSignalAfterLogin(SIGINT);

    // Login Efetuado com sucesso
    createClientStartingThreads(&idEditor, &idMyPipe);
    //A aplicaçao vai terminar...
    exitClient();
}

/**
 * Função responsável por executar o comportamento de numSinal.
 * @param numSinal Código do sinal.
 */
void signalBehaviorBeforeLogin(int numSinal) {
    if (numSinal == SIGINT) {
        exitLoginFailure();
    } else if (numSinal == SIGPIPE) {
        printf("[CLIENTE] Recebi SIGPIPE\n");
        exitServerShutdown();
    }
}

/**
 * Função responsável por redefinir o comportamento de sinal.
 * @param sinal o sinal que o programa recebeu
 */
void configureSignalBeforeLogin(int sinal) {
    if (signal(sinal, signalBehaviorBeforeLogin) == SIG_ERR) {
        perror("Erro a tratar sinal!");
    }
}

/**
 * Função responsável por executar o comportamento de numSinal.
 * @param numSinal Código do sinal.
 */
void signalBehaviorAfterLogin(int numSinal) {
    if (numSinal == SIGINT) {
        pthread_cancel(idEditor);
        puts("[CLIENTE] A thread responsavel pelo editor terminou!");
        exitClient();
    }
}

/**
 * Função responsável por redefinir o comportamento de sinal.
 * @param sinal o sinal que o programa recebeu
 */
void configureSignalAfterLogin(int sinal) {
    if (signal(sinal, signalBehaviorAfterLogin) == SIG_ERR) {
        perror("Erro a tratar sinal!");
    }
}

/**
 * Função responsável por enviar o username para o servidor. Fazendo com que o mesmo seja validado.
 * @param login username do cliente
 */
void sendLoginToServer(char* user) {
    puts("[CLIENTE] Vou enviar login para o servidor!");
    //printf("Estou a abrir o meu pipe %s\n", myPipe);
    fdMyPipe = openNamedPipe(myPipe, O_RDWR);

    //printf("Ja abri o meu pipe\n");

    LoginMsg login;
    strncpy(login.username, user, 9);
    strncpy(login.nomePipeCliente, myPipe, PIPE_MAX_NAME);

    //printf("Estou a escrever no servidor\n");

    int res = write(fdSv, &login, sizeof (login));

    //printf("Ja escrevi\n");

    if (res == -1) {
        fprintf(stderr, "[ERRO]: Nao foi enviado o login para o servidor!\n");
        exitLoginFailure();
    }

    //fdMyPipe = openNamedPipe(myPipe, O_RDONLY);

    ServerMsg msg;
    //printf("Estou a espera da resposta do servidor\n");
    res = read(fdMyPipe, &msg, sizeof (msg));
    //printf("Ja li do servidor\n");

    if (res == -1) {
        fprintf(stderr, "[ERRO]: Nao foi possivel ler a resposta do servidor!\n");
        exitLoginFailure();
    }

    if (msg.code == LOGIN_FAILURE) {
        printf("[CLIENTE] Login Falhou!\n");
        exitLoginFailure();
    }

    closeNamedPipe(fdSv);

    //Abrir pipe interativo
    fdSv = openNamedPipe(msg.intPipeName, O_WRONLY);
    ed = msg.ed;
}

/**
 * Função responsável por criar as threads.
 * @param idEditor Thread idEditor
 * @param idMyPipe Thread idMyPipe
 */
void createClientStartingThreads(pthread_t* idEditor, pthread_t* idMyPipe) {
    int err;
    err = pthread_create(idEditor, NULL, startEditor, NULL);
    if (err)
        printf("[CLIENTE] Nao foi possível criar a thread :[%s]\n", strerror(err));
    else
        printf("[CLIENTE] A thread responsavel pelo editor foi criada!\n");

    err = pthread_create(idMyPipe, NULL, readFromMyPipe, NULL);
    if (err)
        printf("[CLIENTE] Nao foi possível criar a thread :[%s]\n", strerror(err));
    else
        printf("[CLIENTE] A thread responsavel pela leitura do pipe foi criada!\n");
}

/**
 * Função responsável por iniciar o editor.
 * @return NULL
 */
void* startEditor() {
    //TODO mutex
    editor(user, &ed);
    runClient = 0;
    return NULL;
}

/**
 * Função responsável por ler do pipe principal do servidor.
 * @return NULL
 */
void* readFromMyPipe() {
    int nBytes;
    ServerMsg msg;
    int serverUp = 1;

    while (serverUp && runClient) {
        nBytes = read(fdMyPipe, &msg, sizeof (msg));
        if (nBytes == sizeof (msg)) {
            switch (msg.code) {
                case SERVER_SHUTDOWN:
                    serverUp = 0;
                    break;
                case EDITOR_UPDATE:
                    // TODO Terá que ser protegido por Mutex
                    ed = msg.ed;
                    // Atualizar Ecra
                    clearEditor(msg.ed.lin, msg.ed.col);
                    break;
                default:
                    break;
            }
        }
    }
    if (!serverUp)
        exitServerShutdown();
    return NULL;
}

/**
 * Função responsável por fechar a sessão porque o servidor foi desligado, mas fechando primeiramente os pipes e apagando o seu próprio pipe.
 */
void exitServerShutdown() {
    endwin();
    pthread_cancel(idEditor);
    puts("[CLIENTE] A thread responsavel pelo editor terminou!");

    printf("[CLIENTE] O servidor foi desligado!\n[CLIENTE] A aplicação vai encerrar....\n");

    pthread_join(idMyPipe, NULL);
    puts("[CLIENTE] A thread responsavel pela leitura do pipe principal terminou!");

    if (fdMyPipe >= 0) {
        puts("[CLIENTE] Vou fechar o meu pipe!");
        closeNamedPipe(fdMyPipe);
    }

    if (fdSv >= 0) {
        puts("[CLIENTE] Vou fechar o pipe do servidor!");
        closeNamedPipe(fdSv);
    }
    puts("[CLIENTE] Vou apagar o meu pipe!");
    deleteNamedPipe(myPipe);
    exit(0);
}

/**
 * Função responsável por terminar a sessão, avisando o servidor previamente. Fecha os pipes e apaga o pipe principal.
 */
void exitClient() {

    printf("[CLIENTE] A aplicação vai encerrar....\n");
    pthread_join(idEditor, NULL);
    puts("[CLIENTE] A thread responsavel pelo editor terminou!");
    endwin();

    ClientMsg msg;

    msg.msgType = CLIENT_SHUTDOWN;
    strncpy(msg.username, user, 9);

    write(fdSv, &msg, sizeof (msg));

    runClient = 0;
    write(fdMyPipe, "\n", 1);

    pthread_join(idMyPipe, NULL);
    puts("[CLIENTE] A thread responsavel pela leitura do pipe principal terminou!");

    if (fdMyPipe >= 0) {
        puts("[CLIENTE] Vou fechar o meu pipe!");
        closeNamedPipe(fdMyPipe);
    }
    if (fdSv >= 0) {
        puts("[CLIENTE] Vou fechar o pipe do servidor!");
        closeNamedPipe(fdSv);
    }
    puts("[CLIENTE] Vou apagar o meu pipe!");
    deleteNamedPipe(myPipe);

    exit(0);
}

/**
 * Função responsável por terminar a sessão, fechando os pipes abertos e apagando o próprio pipe.
 */
void exitLoginFailure() {
    printf("[CLIENTE] A aplicação vai encerrar....\n");
    if (fdMyPipe >= 0) {
        puts("[CLIENTE] Vou fechar o meu pipe!");
        closeNamedPipe(fdMyPipe);
    }
    if (fdSv >= 0) {
        puts("[CLIENTE] Vou fechar o pipe do servidor!");
        closeNamedPipe(fdSv);
    }
    puts("[CLIENTE] Vou apagar o meu pipe!");
    deleteNamedPipe(myPipe);
    exit(1);
}