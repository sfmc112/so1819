/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   users.h
 * Author: ricardo
 *
 * Created on October 11, 2018, 12:00 PM
 */

#ifndef USERS_H
#define USERS_H

#include "server-functions.h"

#define USERSDEFAULT_DB "medit.db"
#define MAX_NAME 8

int checkUsername(char* user);
int checkUserOnline(char* user, ServerData sd);
int addUsername();

#endif /* USERS_H */
