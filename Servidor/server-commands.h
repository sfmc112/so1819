#ifndef COMMANDS_H
#define COMMANDS_H

void cmdShutdown();
void cmdSettings();
void cmdLoad(char* token);
void cmdSave(char* token);
void cmdFree(char* token);
void cmdStats(int* print);
void cmdUsers();
void cmdText();
int checkCommandArgs(char* token);

// Funções do main (server.c)
void freeLine(int lineNumber);
void printEditor();
void* editorStats(void* param);
void printUsers();
void loadDocument(char* nomeFicheiro);
void saveDocument(char* nomeFicheiro);
void freeOneLine(int lineNumber);
#endif /* COMMANDS_H */
