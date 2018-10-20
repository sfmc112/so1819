#include "server-functions.h"
#include "users.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

/**
 * Função para verificar se existem argumentos de inicialização do programa.
 * @param argc número de argumentos
 * @param argv argumentos
 * @param sd estrutura das configurações principais do servidor
 */
void checkArgs(int argc, char** argv, ServerData* sd) {
    if (argc == 3) {
        char* cmd;
        int res;

        while ((res = getopt(argc, argv, "f")) != -1) {
            switch (res) {
                case 'f':
                    cmd = optarg;
                    if (ifFileExists(cmd))
                        strncpy(sd->usersDB, cmd, MAX_SIZE_FILENAME);
                    else
                        strncpy(sd->usersDB, USERSDEFAULT_DB, MAX_SIZE_FILENAME);
                    break;
            }
        }
    } else {
        strncpy(sd->usersDB, USERSDEFAULT_DB, MAX_SIZE_FILENAME);
    }
}

void defineMaxUsers(ServerData* sd){
    //Definir número máximo de utilizadores
    sd->maxUsers = MAX_USERS;
}

/**
 * Inicializa as estruturas do servidor (variáveis globais)
 * as variáveis de linha/coluna já foram atríbuídas assim como o nome do ficheiro
 * da base de dados dos utilizadores
 */
void initializeMEDITLines(EditorData* ed) {
    //Definir número máximo de utilizadores
    sData->maxUsers = MAX_USERS;
    
    //Inicializar linhas
    int i, j;

    for (i = 0; i < ed->lin; i++) {
        ed->lines[i].free = 1;
        for (j = 0; j < ed->col; j++)
            ed->lines[i].text[j] = ' ';
    }
}

/**
 * Função que verifica se existem as variáveis de ambiente 
 * MEDIT_MAXLINES e MEDIT_MAXCOLUMNS e vai buscar os seus valores, redefinindo
 * as variáveis presentes na estrutura EditorData caso sejam inferiores aos
 * limites máximos
 * @param ed estrutura de dados com informação do editor de texto
 */
void getEnvironmentVariables(EditorData* ed) {
    char* l, c;
    int lin, col;

    l = getenv(VAR_AMBIENTE_LINHAS);

    if (l != NULL) {
        lin = atoi(l);
        if (lin < VAR_MAXLINES)
            ed->lin = lin;
        else
            ed->lin = VAR_MAXLINES;
    }

    c = getenv(VAR_AMBIENTE_COLUNAS);

    if (c != NULL) {
        col = atoi(c);
        if (col < VAR_MAXCOLUMNS)
            ed->col = col;
        else
            ed->col = VAR_MAXCOLUMNS;
    }
}