#ifndef SERVER_DEFAULTS_H
#define SERVER_DEFAULTS_H

#include "biblioteca.h"
#define MAX_SIZE_FILENAME 50

typedef struct _clientdata {
    // Estado do cliente:\n - Ativo = 1\n - Inativo = 0
    int valid;
    // Nome de utilizador
    char username[9];
    // Descritor do named pipe interativo que foi atribuído ao cliente
    int fdIntPipe;
    // Descritor do named pipe do cliente
    int fdPipeClient;
    // Linha do editor onde o cliente se encontra
    int linePosition;
    // Coluna do editor onde o cliente se encontra
    int columnPosition;
} ClientData;

typedef struct _serverdata {
    // Variável de controlo de execução do servidor
    int runServer;
    // Nome do ficheiro da base dados de utilizadores
    char usersDB[MAX_SIZE_FILENAME];
    // Número máximo de utilizadores
    int maxUsers;
    // Named Pipe Principal
    char mainPipe[PIPE_NAME_MAX];
    // Número de named pipes interativos (POOL)
    int numInteractivePipes;
    // Clientes
    ClientData clients[DEFAULT_MAXUSERS];
} ServerData;

typedef struct _intpipe {
    // Número de utilizadores ligados a este pipe
    int numUsers;
    // Nome do pipe de interação
    char pipeName[PIPE_NAME_MAX];
    // Descritor do pipe interactivo
    int fd;
} InteractionPipe;

#endif /* SERVER_DEFAULTS_H */

