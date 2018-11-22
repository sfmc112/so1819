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
#define PIPE_SERVER "./Pipes/pipe_server_"
#else
#define PIPE_USER "../Pipes/pipe_user_"
#define PIPE_SERVER "../Pipes/pipe_server_"
#endif

#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H


#ifdef __cplusplus
extern "C" {
#endif

#define PIPE_NAME_MAX 50
int createNamedPipe(char* nomePipe, char* dono);
int deleteNamedPipe(char* pathname);
int openNamedPipe(char* pathname, int mode);
int closeNamedPipe(int fd);
#ifdef __cplusplus
}
#endif


#endif /* BIBLIOTECA_H */

