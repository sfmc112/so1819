#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#ifdef NO_IDE
#define PIPE_USER "./Pipes/pipe_user_"
#define MAIN_PIPE_SERVER "./Pipes/main_pipe_server"
#define INTERACTIVE_PIPE_SERVER "./Pipes/int_pipe_server_"
#else
#define PIPE_USER "../Pipes/pipe_user_"
#define MAIN_PIPE_SERVER "../Pipes/main_pipe_server"
#define INTERACTIVE_PIPE_SERVER "../Pipes/int_pipe_server_"
#endif

#define PIPE_NAME_MAX 50
#define NUM_INTERACTIVE_PIPES 5


int createNamedPipe(char* nomePipe, char* dono);
int createServerNamedPipe();
int deleteNamedPipe(char* pathname);
int openNamedPipe(char* pathname, int mode);
int closeNamedPipe(int fd);


#endif /* BIBLIOTECA_H */

