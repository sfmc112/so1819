#ifndef UTILS_H
#define UTILS_H
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
#endif /* UTILS_H */
