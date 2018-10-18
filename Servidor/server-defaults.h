#ifndef SERVER_DEFAULTS_H
#define SERVER_DEFAULTS_H
#define MAXSIZE 50
#include "../medit-defaults.h"
typedef struct _serverdata ServerData;

struct _serverdata{
    char usersDB[MAXSIZE]; // Nome do ficheiro da base dados de utilizadores
    //int maxUsers;
};
#endif /* SERVER_DEFAULTS_H */

