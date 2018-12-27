#ifndef USERS_H
#define USERS_H

#include "server-functions.h"

#define USERSDEFAULT_DB "medit.db"
#define MAX_NAME 8

int checkUsername(char* user);
int checkUserOnline(char* user, ServerData sd);
int addUsername();

#endif /* USERS_H */
