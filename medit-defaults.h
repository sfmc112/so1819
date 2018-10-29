#ifndef MEDIT_DEFAULTS_H
#define MEDIT_DEFAULTS_H

#define VAR_AMBIENTE_LINHAS "MEDIT_MAXLINES"
#define VAR_AMBIENTE_COLUNAS "MEDIT_MAXCOLUMNS"
#define VAR_AMBIENTE_TIMEOUT "MEDIT_TIMEOUT"
#define VAR_AMBIENTE_USERS "MEDIT_MAXUSERS"

#define DEFAULT_MAXLINES 15
#define DEFAULT_MAXCOLUMNS 45
#define DEFAULT_TIMEOUT 10
#define DEFAULT_MAXUSERS 3

//Definição das estruturas de dados



typedef struct _line {
    int free; // 1 se está livre, 0 se está ocupado por um utilizador
    char text[DEFAULT_MAXCOLUMNS]; // Representa uma linha de texto
} Line;

typedef struct _editordata EditorData;

struct _editordata {
    int lin; // Número de linhas que o editor possui
    int col; // Comprimento de uma linha de texto
    int timeout; //tempo de inatividade após o qual uma linha é libertada
    Line lines[DEFAULT_MAXLINES]; // As linhas com o texto
};

#endif
