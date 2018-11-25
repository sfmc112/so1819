#ifndef MEDIT_DEFAULTS_H
#define MEDIT_DEFAULTS_H

#include "Biblioteca/biblioteca.h"

#define VAR_AMBIENTE_LINHAS "MEDIT_MAXLINES"
#define VAR_AMBIENTE_COLUNAS "MEDIT_MAXCOLUMNS"
#define VAR_AMBIENTE_TIMEOUT "MEDIT_TIMEOUT"
#define VAR_AMBIENTE_USERS "MEDIT_MAXUSERS"

#define MAX_FILE_NAME 50

#define DEFAULT_MAXLINES 15
#define DEFAULT_MAXCOLUMNS 45
#define DEFAULT_TIMEOUT 10
#define DEFAULT_MAXUSERS 15

/* Code */
#define LOGIN_SUCCESS 0
#define LOGIN_FAILURE 1
#define SERVER_SHUTDOWN 2
#define EDITOR_UPDATE 3

/* MSG TYPES */
#define MOVE_UP 100
#define MOVE_DOWN 101
#define MOVE_LEFT 102
#define MOVE_RIGHT 103
#define K_ENTER 104
#define K_ESC 105
#define K_DEL 106
#define K_BACKSPACE 107
#define K_CHAR 108

//Definição das estruturas de dados

typedef struct _line {
    int free; // 1 se está livre, 0 se está ocupado por um utilizador
    char text[DEFAULT_MAXCOLUMNS]; // Representa uma linha de texto
} Line;

typedef struct _editordata EditorData;

struct _editordata {
    char fileName[MAX_FILE_NAME]; //nome do ficheiro de texto
    int lin; // Número de linhas que o editor possui
    int col; // Comprimento de uma linha de texto
    int timeout; //tempo de inatividade após o qual uma linha é libertada
    Line lines[DEFAULT_MAXLINES]; // As linhas com o texto
};

typedef struct _servermessage {
    EditorData ed;
    int code; // 0 caso nao haja erro, 1 caso contrário
    char intPipeName[PIPE_NAME_MAX];
} ServerMsg;

typedef struct _clientmessage {
    int msgType; //tipo de mensagem
    char letra;
    char username[9];
} ClientMsg;

typedef struct _loginmsg {
    char username[9];
    char nomePipeCliente[PIPE_NAME_MAX];
} LoginMsg;

#endif
