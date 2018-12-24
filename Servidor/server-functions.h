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
//int getClientPipe(ServerData sd, char* user);
int getClientArrayPosition(ServerData sd, char* user);
void moveAllToTheLeft(char* linha, int x, int max_x);
int moveAllToTheRight(char* linha, int x, int max_x);
int getPercentage(char* user, EditorData ed);
void getUsersOrderedBySessionDuration(int* users, ClientData* clients, int size);
void sendMessageEditorUpdateToAllClients(EditorData ed, ServerData sd);
void writeToAllClients(ServerData sd, ServerMsg smsg);
#endif /* SERVER_FUNCTIONS_H */
