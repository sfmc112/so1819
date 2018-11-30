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
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PIPE_PATRAO "/tmp/pipe_patrao"
#define PIPE_EMP "/tmp/pipe_emp_"

/*
 * 
 */
int main(int argc, char** argv) {
    int pid = getpid();
    char pipe[25];
    snprintf(pipe, 25, "%s%d", PIPE_EMP, pid);
    if (mkfifo(pipe, O_RDONLY) == -1) {
        fprintf(stderr, "[CLIENTE]: Erro ao criar o pipe para leitura!\n");
        return EXIT_FAILURE;
    }
    
    printf("[CLIENTE] Criei o pipe para leitura!\n");
    
    int fdEmp;
    if ((fdEmp = open(pipe, O_RDONLY)) == -1) {
        fprintf(stderr, "[CLIENTE]: Erro ao abrir o pipe para leitura!\n");
        return EXIT_FAILURE;
    }
    
    printf("[CLIENTE] Abri o pipe para leitura!\n");
    // TODO MORE

    int fdServ;
    if ((fdServ = open(PIPE_PATRAO, O_WRONLY)) == -1) {
        fprintf(stderr, "[CLIENTE]: Erro ao abrir o pipe para escrita!\n");
        return EXIT_FAILURE;
    }
    
    printf("[CLIENTE] Abri o pipe para escrita!\n");
    
    write(fdServ, &pid, sizeof (int));
    close(fdServ);
    getchar();
    return EXIT_SUCCESS;
}

