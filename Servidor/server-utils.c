#include "server-utils.h"
#include <stdio.h>
//#include <signal.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Função que termina a aplicação com erro.
 * @param mensagem mensagem de erro
 */
void exitError(char* mensagem) {
    perror(mensagem);
    exit(1);
}

void exitNormal(){
    //Fazer outras coisas
    exit(0);
}

/**
 * Função para colocar toda a string em letras minusculas.
 * @param buffer string a ser transformada
 * @return 
 */
void toLower(char* buffer) {
    for (int i = 0; buffer[i] != '\0'; i++)
        buffer[i] = tolower(buffer[i]);
}

/**
 * Função para colocar toda a string em letras maiusculas.
 * @param buffer string a ser transformada
 * @return 
 */
void toUpper(char* buffer) {
    for (int i = 0; buffer[i] != '\0'; i++)
        buffer[i] = toupper(buffer[i]);
}

/**
 * Função que verifica se o dado ficheiro existe
 * @param nomeFicheiro caminho/nome do ficheiro
 * @return 1 se existe e 0 caso contrário
 */
int ifFileExists(char* nomeFicheiro) {
    FILE* f;
    f = fopen(nomeFicheiro, "r");
    if (f == NULL)
        return 0;
    fclose(f);
    return 1;
}

/*
char* getSignalName(int sig) {
    char *str = strdup(sys_signame[sig]);
    if (!str)
        return -1;

    toUpper(str);
    printf("%2d -> SIG%s\n", sig, str);

    free(str);
}
*/

