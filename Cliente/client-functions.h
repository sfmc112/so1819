#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H
void checkArgs(int argc, char** argv);
void loginSession();
void editor();
void editMode(char* text, int posx, int posy, int ncol, char cursor);
#endif /* CLIENT_FUNCTIONS_H */
