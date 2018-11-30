/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: sarah
 *
 * Created on November 5, 2018, 3:08 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PIPE_PATRAO "/tmp/pipe_patrao"
#define PIPE_EMP "/tmp/pipe_emp_"

/*
 * 
 */
int main(int argc, char** argv) {
    int fdPat, fdEmp, continua;
    int pid, numero;

    //Criar pipe
    if (mkfifo(PIPE_PATRAO, O_RDONLY) == -1) {
        fprintf(stderr, "[PATRAO]: Erro a criar pipe para leitura.\n");
        return EXIT_FAILURE;
    }

    //Abrir pipe para leitura
    fdPat = open(PIPE_PATRAO, O_RDONLY);
    if (fdPat == -1) {
        fprintf(stderr, "[PATRAO]: Erro a abrir pipe para leitura.\n");
        return EXIT_FAILURE;
    }

    //Esperar pelo empregado
    continua = read(fdPat, &pid, sizeof (int));
    if (continua <= 0) {
        fprintf(stderr, "[PATRAO]: Nao há mais empregado.\n");
        close(fdPat);
        return EXIT_FAILURE;
    }

    char pipeEmp[25];

    printf("[PATRAO]:O meu empregado e o %d, vou lhe criar um pipe para poder falar com ele\n", pid);

    sprintf(pipeEmp, "%s%d", PIPE_EMP, pid);


    //Abrir pipe do empregado para escrita
    fdEmp = open(pipeEmp, O_WRONLY);
    if (fdEmp == -1) {
        fprintf(stderr, "[PATRAO]: Erro a abrir pipe para escrever no empregado.\n");
        return EXIT_FAILURE;
    }
    
    printf("[PATRAO]:O pipe %s foi criado.\n", pipeEmp);
    
    getchar();

/*
    char comando[50];
    int nBytes;

    do {
        printf("Escreve comando para o empregado: ");
        scanf(" %49[^\n]", comando);
        
        //nBytes = write
        
    } while (strcmp(comando, "sair") != 0 || nBytes == -1);
*/
    
    fclose(fdEmp);
    fclose(fdPat);

    return (EXIT_SUCCESS);
}

