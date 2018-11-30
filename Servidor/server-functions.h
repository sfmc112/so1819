/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   server-functions.h
 * Author: ricardo
 *
 * Created on October 18, 2018, 11:31 AM
 */

#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H
#include "server-defaults.h"

void checkArgs(int argc, char** argv, ServerData* sd);
void getEnvironmentVariables(EditorData* ed, ServerData* sd);
void resetMEDITLines(EditorData* ed);
void initializeServerData(ServerData* sd);
int getFirstAvailablePosition(ServerData sd);
void registerClient(char* username, ServerData* sd, int pos, int fdCli, int fdIntPipe);
int getIntPipe(ServerData sd, InteractionPipe* pipes);
void closeAndDeleteServerPipes(int fdMainPipe, ServerData* sd, InteractionPipe* pipes);
void removeClient(char* username, ServerData* sd);
#endif /* SERVER_FUNCTIONS_H */
