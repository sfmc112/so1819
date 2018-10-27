#ifndef UTILS_H
#define UTILS_H
void exitError(char* mensagem);
void exitNormal();
void toLower(char* buffer);
void toUpper(char* buffer);
int ifFileExists(char* nomeFicheiro);
//char* getSignalName(int signal, char* str);
#endif /* UTILS_H */
