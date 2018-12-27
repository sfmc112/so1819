#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H

#ifdef NO_IDE
#include "medit-defaults.h"
#else
#include "../medit-defaults.h"
#endif


int checkArgs(int argc, char** argv, char* pipeName, char* user);
void loginSession();
void editor(char* user, EditorData * ed, int fdCli, int fdServ, int* sair);
void editMode(int y, int x, char* linha);
void clearEditor(int dimY, int dimX);
void refreshCursor(int y, int x, int lines);
void writeDocument(Line *text, int nLines);
void writeUsers(EditorData ed);
void exitServerShutdown();
void exitClient();

#endif /* CLIENT_FUNCTIONS_H */
