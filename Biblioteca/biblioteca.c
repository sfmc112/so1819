/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: sarah
 *
 * Created on November 12, 2018, 3:31 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "biblioteca.h"

int fatalErrorMsg(char* descricao, char* funcao);

/**
 * Função responsável por criar o named pipe.
 * @param nomePipe string vazia que vai ser atualizada com o nome do pipe
 * @param dono parte inicial do nome do pipe (sem pid)
 * @return 0 se correu bem, EXIT_FAILURE caso contrário
 */
int createNamedPipe(char* nomePipe, char* dono) {
    int pid = getpid();
    snprintf(nomePipe, PIPE_NAME_MAX, "%s%d", dono, pid);
    if (mkfifo(nomePipe, S_IRUSR | S_IWUSR) == -1) {
        if (errno == EEXIST)
            printf("A fifo <%s> existente sera usada!\n", nomePipe);
        else {
            return fatalErrorMsg("Nao consegui criar o pipe", "createNamedPipe()");
        }
    }
    printf("O pipe <%s> foi criado com sucesso!\n", nomePipe);
    return 0;
}

/**
 * Função responsável por criar o named pipe do servidor.
 * @return 0 se correu bem, EXIT_FAILURE caso contrário
 */
int createServerNamedPipe(char* pipeName) {
    if (mkfifo(pipeName, S_IRUSR | S_IWUSR) == -1) {
        printf("\n%s\n\n", pipeName);
        if (errno == EEXIST)
            printf("A fifo <%s> existente sera usada!\n", pipeName);
        else {
            return fatalErrorMsg("Nao consegui criar o pipe", "createNamedPipe()");
        }
    }
    printf("O pipe <%s> foi criado com sucesso!\n", pipeName);
    return 0;
}

/**
 * Função responsável por efetuar o unlink do named pipe.
 * @param pathname nome do named pipe.
 */
int deleteNamedPipe(char* pathname) {
    int res = unlink(pathname);
    if (res == -1) {
        return fatalErrorMsg("Nao consegui eliminar o pipe", "deleteNamedPipe()");
    }
    printf("O pipe <%s> foi eliminado com sucesso!\n", pathname);
    return 0;
}

/**
 * Função responsável por abrir o named pipe.
 * @param pathname nome do named pipe.
 * @param mode modo de abertura do pipe (leitura ou escrita).
 * @return 
 */
int openNamedPipe(char* pathname, int mode) {
    int fd = open(pathname, mode);
    if (fd == -1) {
        return fatalErrorMsg("Nao consegui abrir o pipe para escrita", "openNamedPipe()");
    }
    printf("O pipe <%s> foi aberto com sucesso!\n", pathname);
    return fd;
}

/**
 * Função responsável por fechar o named pipe.
 * @param pathname nome do named pipe.
 */
int closeNamedPipe(int fd) {
    int res = close(fd);
    if (res == -1) {
        return fatalErrorMsg("Nao consegui fechar o pipe", "closeNamedPipe()");
    }
    return 0;
}

void writeServerMsg(int fd, ServerMsg* msg){
    write(fd, msg, sizeof(msg));
}

/**
 * Função responsável por notificar a função que a chamou com um erro grave.
 * @param descricao tarefa não executada por causa de um erro
 * @param funcao nome da função que chamou
 * @return erro grave (1)
 */
int fatalErrorMsg(char* descricao, char* funcao) {
    fprintf(stderr, "[ERRO]: %s\n", descricao);
    perror(funcao);
    return -1;
}