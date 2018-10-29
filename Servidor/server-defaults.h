#ifndef SERVER_DEFAULTS_H
#define SERVER_DEFAULTS_H

#ifdef NO_IDE
    #include "medit-defaults.h"
#else
    #include "../medit-defaults.h"
#endif

#define MAX_SIZE_FILENAME 50

typedef struct _serverdata ServerData;

struct _serverdata{
    // Nome do ficheiro da base dados de utilizadores
    char usersDB[MAX_SIZE_FILENAME]; 
    //Número máximo de utilizadores
    int maxUsers;
};

#endif /* SERVER_DEFAULTS_H */

