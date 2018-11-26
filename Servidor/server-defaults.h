#ifndef SERVER_DEFAULTS_H
#define SERVER_DEFAULTS_H

#include "biblioteca.h"
#define MAX_SIZE_FILENAME 50

typedef struct _clientdata {
    int valid;
    char username[9];
    int fdIntPipe;
    int fdPipeClient;
} ClientData;

typedef struct _serverdata {
    //Variável de controlo de execução do servidor
    int runServer;
    // Nome do ficheiro da base dados de utilizadores
    char usersDB[MAX_SIZE_FILENAME];
    //Número máximo de utilizadores
    int maxUsers;
    // Named Pipe Principal
    char mainPipe[PIPE_NAME_MAX];
    // Número de named pipes interativos (POOL)
    int numInteractivePipes;
    // Clientes
    ClientData clients[DEFAULT_MAXUSERS];
} ServerData;

typedef struct _intpipe {
    //Número de utilizadores ligados a este pipe
    int numUsers;
    //Nome do pipe de interação
    char pipeName[PIPE_NAME_MAX];
    //Descritor do pipe interactivo
    int fd;
} InteractionPipe;

#endif /* SERVER_DEFAULTS_H */

