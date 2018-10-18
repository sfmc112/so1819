#ifndef MEDIT_DEFAULTS_H
#define MEDIT_DEFAULTS_H


#define VAR_MAXLINES 15
#define VAR_MAXCOLUMNS 45

//Definição das estruturas de dados

typedef struct _editordata EditorData;

typedef struct _line{
    int free; // 1 se está livre, 0 se está ocupado por um utilizador
    int col; // Comprimento de uma linha de texto
    char text[VAR_MAXCOLUMNS]; // Representa uma linha de texto
} Line;

struct _editordata{
    int lin; // Número de linhas que o editor possui
    Line lines[VAR_MAXLINES]; // As linhas com o texto
};

#endif
