/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: sarah
 *
 * Created on November 22, 2018, 1:25 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int contaPalavras(char * msg);
int contaAsteriscos(char* msg);

/*
 * 
 */
int main(int argc, char** argv) {
    int pidPai = getpid();
    int pidFilho;
    int fdPipeToAspell[2];
    int fdPipeFromAspell[2];
    int estado;

    pipe(fdPipeToAspell);
    pipe(fdPipeFromAspell);

    pidFilho = fork();

    if (pidFilho == 0) {
        pidFilho = getpid();
        dup2(fdPipeToAspell[0], STDIN_FILENO);
        close(fdPipeToAspell[1]);
        dup2(fdPipeFromAspell[1], STDOUT_FILENO);
        close(fdPipeFromAspell[0]);
        execlp("aspell", "aspell", "-a", "-d", "pt_PT", NULL);
    } else {
        close(fdPipeToAspell[0]);
        close(fdPipeFromAspell[1]);

        char msg[1024];
        int bytesRead;
        char resp[4096];
        int numPalavras;
        int numAstericos;
        int i;

        bytesRead = read(fdPipeFromAspell[0], resp, 4096);
        resp[bytesRead - 1] = 0;
        printf("Aspell: <%s>\n", resp);

        while (1) {
            fgets(msg, 1024, stdin);
            numPalavras = contaPalavras(msg);
            //msg[strlen(msg) - 1] = '\0';
            if (strncmp(msg, "exit", 4) == 0) {
                break;
            }
            write(fdPipeToAspell[1], msg, strlen(msg));
            bytesRead = read(fdPipeFromAspell[0], resp, 4096);
            resp[bytesRead - 1] = 0;
            printf("\nAspell: <%s>\n\n", resp);
            //msg[strlen(msg) - 1] = '\0';

            numAstericos = contaAsteriscos(resp);

            if (numPalavras == numAstericos)
                fprintf(stdout, "A frase <%s> esta correta\n", msg);
            else
                fprintf(stdout, "Houve um erro na frase <%s>!\n", msg);

        }
        close(fdPipeToAspell[1]);
        wait(&estado);
        return (EXIT_SUCCESS);
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

