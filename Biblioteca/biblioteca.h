/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   biblioteca.h
 * Author: ricardo
 *
 * Created on November 12, 2018, 4:28 PM
 */


#ifdef NO_IDE
#define PIPE_USER "./Pipes/pipe_user_"
#define MAIN_PIPE_SERVER "./Pipes/main_pipe_server_"
#define INTERACTIVE_PIPE_SERVER "./Pipes/int_pipe_server_"
#else
#define PIPE_USER "../Pipes/pipe_user_"
#define MAIN_PIPE_SERVER "../Pipes/main_pipe_server_"
#define INTERACTIVE_PIPE_SERVER "../Pipes/int_pipe_server_"
#endif

#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#define PIPE_NAME_MAX 50
#define NUM_INTERACTIVE_PIPES 5


int createNamedPipe(char* nomePipe, char* dono);
int deleteNamedPipe(char* pathname);
int openNamedPipe(char* pathname, int mode);
int closeNamedPipe(int fd);


#endif /* BIBLIOTECA_H */

