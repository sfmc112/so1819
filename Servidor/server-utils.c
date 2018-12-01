#include "server-utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int contaPalavras(char * msg);
int contaAsteriscos(char* msg);

/**
 * Função que termina a aplicação com erro.
 * @param mensagem mensagem de erro
 */
void errorMessage(char* message) {
    perror(message);
}

void exitError(char* mensagem) {
    errorMessage(mensagem);
    exit(1);
}

//void exitNormal() {
//    exit(0);
//}

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
    if (f == NULL) {
        errorMessage("Ficheiro nao existe!");
        return 0;
    }
    fclose(f);
    return 1;
}

int verifySingleInstance() {
    return open("/tmp/unique.txt", O_RDONLY | O_EXCL | O_CREAT);
}

void startAspell(int* fdWrite, int* fdRead) {
    int fdPipeToAspell[2];
    int fdPipeFromAspell[2];

    pipe(fdPipeToAspell);
    pipe(fdPipeFromAspell);


    if (fork() == 0) {
        dup2(fdPipeToAspell[0], STDIN_FILENO);
        close(fdPipeToAspell[1]);
        dup2(fdPipeFromAspell[1], STDOUT_FILENO);
        close(fdPipeFromAspell[0]);
        execlp("aspell", "aspell", "-a", "-d", "pt_PT", NULL);
    } else {
        close(fdPipeToAspell[0]);
        close(fdPipeFromAspell[1]);
        *fdWrite = fdPipeToAspell[1];
        *fdRead = fdPipeFromAspell[0];

        int bytesRead;
        char resp[4096];
        int numPalavras;
        int numAstericos;
        char pal1[50] = "palavra";
        char pal2[50] = "sarah";

        bytesRead = read(fdPipeFromAspell[0], resp, 4096);
        resp[bytesRead - 1] = 0;
        printf("Aspell: <%s>\n", resp);

        numPalavras = contaPalavras(pal1);
        //msg[strlen(msg) - 1] = '\0';
        write(fdPipeToAspell[1], pal1, strlen(pal1));
        bytesRead = read(fdPipeFromAspell[0], resp, 4096);
        resp[bytesRead - 1] = 0;
        printf("\nAspell:\n%s\n\n", resp);
        pal2[strlen(pal2) - 1] = '\0';
        //TODO ACABAR ISTO

        numAstericos = contaAsteriscos(resp);

        if (numPalavras == numAstericos)
            fprintf(stdout, "A frase esta correta\n");
        else
            fprintf(stdout, "Houve um erro na frase!\n");

    }
}

int contaPalavras(char * msg) {
    char tempMsg[1024];
    strncpy(tempMsg, msg, 1024);
    char* token;
    int conta = 0;

    token = strtok(tempMsg, " .,;:-_?!");
    conta++;

    while ((token = strtok(NULL, " .,;:-_?!")) != NULL)
        conta++;

    return conta;
}

int contaAsteriscos(char* msg) {
    int i = 0, conta = 0;
    //msg[strlen(msg)] = '\0';
    while (msg[i] != '\0') {
        if (msg[i] == '*')
            conta++;
        i++;
    }

    return conta;
}

