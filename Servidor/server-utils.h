#ifndef UTILS_H
#define UTILS_H

#ifdef NO_IDE
#include "medit-defaults.h"
#else
#include "../medit-defaults.h"
#endif

void exitError(char* mensagem);
void errorMessage(char* message);
void exitNormal();
void toLower(char* buffer);
void toUpper(char* buffer);
int ifFileExists(char* nomeFicheiro);
int verifySingleInstance();
void startAspell(int* fdWrite, int* fdRead);
int spellCheck(char* msg, int fdWrite, int fdRead);
int spellCheckSentence(char * msg, int fdWrite, int fdRead);
int isLineEmpty(char* line, int tam);
int contaPalavras(char * msg);
char* getArrayOfUniqueChars(EditorData eData, int* numUniqueChars);
int countChars(EditorData eData, char c);
int doesCharExistInArray(char* a, int size, char c);
#endif /* UTILS_H */
