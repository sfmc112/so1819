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
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

/*
 * 
 */
int main(int argc, char** argv) {
    int pidPai = getpid();
    int pidFilho;
    int fdpipe[2];
    int estado;
    
    pipe(fdpipe);
    
    if(fork() == 0){
        char mensagem[256];
        int nBytes;
        dup2(fdpipe[0], 0);
        execlp("aspell", "aspell", "-a", "-d", "pt_PT", "NULL");
        printf("%d\n", errno);
        nBytes = read(fdpipe[0], mensagem, 256);
        mensagem[nBytes] = '\0';
        printf("%s\n", mensagem);
        //close(fdpipe[0]);

    }else{
        dup2(fdpipe[1], 1);
       char* msg = "Sou o pai e estou bem";
        write(fdpipe[1], msg, strlen(msg));
        //close(fdpipe[1]);
        wait(&estado);
    }
    
    

    return (EXIT_SUCCESS);
}

