/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: ricardo
 *
 * Created on November 23, 2018, 9:29 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// Threads
#include <pthread.h>
#include <string.h>
#include "biblioteca.h"
#define MAX_NAME 8
/*
 * 
 */
void* readMainPipe(void * arg);
void createNamedPipesServer();
void* readCommands();
pthread_t tid[2];

typedef struct {
    char buffer[MAX_NAME];
    int fdMP;
} TDados;

int main(int argc, char** argv) {
    char pipeName[PIPE_NAME_MAX];
    TDados worker;

    createNamedPipesServer(pipeName);
    worker.fdMP = openNamedPipe(pipeName, O_RDWR);
    int err;
    err = pthread_create(&(tid[0]), NULL, readCommands, NULL);
    if (err) printf("\nCan't create thread :[%s]", strerror(err));
    else printf("\n Thread created successfully\n");

    err = pthread_create(&(tid[1]), NULL, readMainPipe, (void *) &worker);
    if (err) printf("\nCan't create thread :[%s]", strerror(err));
    else printf("\n Thread created successfully\n");

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    sleep(5);
    return (EXIT_SUCCESS);
}

void* readMainPipe(void * arg) {
    int bytes;
    TDados * dados = (TDados *) arg;

    do {
        bytes = read(dados->fdMP, dados->buffer, MAX_NAME);
    } while (bytes != 0);

    fprintf(stdout, "%s\n", dados->buffer);
    return NULL;
}

void createNamedPipesServer(char* pipeName) {
    createNamedPipe(pipeName, "Main_Pipe");

    char temp[PIPE_NAME_MAX];
    for (int i = 0; i < 5; i++) {
        snprintf(temp, PIPE_NAME_MAX, "%s_%d_", INTERACTIVE_PIPE_SERVER, i);
        createNamedPipe(pipeName, temp);
    }
}

void* readCommands() {
    getchar();
}