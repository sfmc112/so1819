#ifndef SERVER_DEFAULTS_H
#define SERVER_DEFAULTS_H
#include "../medit-defaults.h"
typedef _serverdata ServerData;

struct _serverdata{
    char* usersDB; // Nome do ficheiro da base dados de utilizadores
    //int maxUsers;
};
#endif /* SERVER_DEFAULTS_H */

