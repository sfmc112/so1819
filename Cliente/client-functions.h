#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H
void checkArgs(int argc, char** argv, char* pipeName, char* user);
void loginSession();
void editor(char* user);
void editMode(int y, int x, char* linha);
int sendLoginToServer(char* login);
#endif /* CLIENT_FUNCTIONS_H */
