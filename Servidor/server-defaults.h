#ifndef SERVER_DEFAULTS_H
#define SERVER_DEFAULTS_H


#include "../medit-defaults.h"

#define MAX_SIZE_FILENAME 50

typedef struct _serverdata ServerData;

struct _serverdata{
    char usersDB[MAX_SIZE_FILENAME]; // Nome do ficheiro da base dados de utilizadores
    int maxUsers;
};
#endif /* SERVER_DEFAULTS_H */

