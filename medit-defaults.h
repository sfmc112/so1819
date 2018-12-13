#ifndef MEDIT_DEFAULTS_H
#define MEDIT_DEFAULTS_H


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
#define EDITOR_SHUTDOWN 4

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
#define CLIENT_SHUTDOWN 109

#define PIPE_MAX_NAME 50

//Definição das estruturas de dados

typedef struct _line {
    int free; // 1 se está livre, 0 se está ocupado por um utilizador
    char text[DEFAULT_MAXCOLUMNS]; // Representa uma linha de texto
} Line;

typedef struct _editordata EditorData;

struct _editordata {
    // Nome do ficheiro de texto
    char fileName[MAX_FILE_NAME];
    // Número de linhas que o editor possui
    int lin;
    // Comprimento de uma linha de texto
    int col;
    // Tempo de inatividade após o qual uma linha é libertada
    int timeout;
    // As linhas com o texto
    Line lines[DEFAULT_MAXLINES];
    int cursorLinePosition;
    int cursorColumnPosition;
};

typedef struct _servermessage {
    // Códigos disponíveis:\n\n - LOGIN_SUCCESS 0\n - LOGIN_FAILURE 1\n - SERVER_SHUTDOWN 2\n - EDITOR_UPDATE 3\n - EDITOR_SHUTDOWN 4
    int code;
    char intPipeName[PIPE_MAX_NAME];
    EditorData ed;

} ServerMsg;

typedef struct _clientmessage {
    // Tipo de mensagem:\n\n - MOVE_UP 100\n - MOVE_DOWN 101\n - MOVE_LEFT 102\n - MOVE_RIGHT 103\n - K_ENTER 104\n - K_ESC 105\n - K_DEL 106\n - K_BACKSPACE 107\n - K_CHAR 108\n - CLIENT_SHUTDOWN 109
    int msgType;
    char letra;
    char username[9];
    int linePosition;
    int columnPosition;
} ClientMsg;

typedef struct _loginmsg {
    char username[9];
    char nomePipeCliente[PIPE_MAX_NAME];
} LoginMsg;

#endif
