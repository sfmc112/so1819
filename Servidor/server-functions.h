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
int getClientArrayPosition(ServerData sd, char* user);
void moveAllToTheLeft(char* linha, int x, int max_x);
int moveAllToTheRight(char* linha, int x, int max_x);
int getPercentage(char* user, EditorData ed);
void getUsersOrderedBySessionDuration(int* users, ClientData* clients, int size);
void sendMessageEditorUpdateToAllClients(EditorData ed, ServerData sd);
void writeToAllClients(ServerData sd, ServerMsg smsg);
int countNumberOfWords(EditorData eData);
int countNumberofLetters(EditorData eData);
void getMostCommonChars(char* mostCommonChars, EditorData eData);

#endif /* SERVER_FUNCTIONS_H */
