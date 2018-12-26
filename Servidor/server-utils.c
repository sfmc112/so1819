#include "server-utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int contaAsteriscos(char* msg);
int countNewLines(char* buffer);

/**
 * Função que termina a aplicação com erro.
 * @param mensagem mensagem de erro
 */
void errorMessage(char* message) {
    perror(message);
}

void exitError(char* mensagem) {
    errorMessage(mensagem);
    exit(1);
}

//void exitNormal() {
//    exit(0);
//}

/**
 * Função para colocar toda a string em letras minusculas.
 * @param buffer string a ser transformada
 * @return 
 */
void toLower(char* buffer) {
    for (int i = 0; buffer[i] != '\0'; i++)
        buffer[i] = tolower(buffer[i]);
}

/**
 * Função para colocar toda a string em letras maiusculas.
 * @param buffer string a ser transformada
 * @return 
 */
void toUpper(char* buffer) {
    for (int i = 0; buffer[i] != '\0'; i++)
        buffer[i] = toupper(buffer[i]);
}

/**
 * Função que verifica se o dado ficheiro existe
 * @param nomeFicheiro caminho/nome do ficheiro
 * @return 1 se existe e 0 caso contrário
 */
int ifFileExists(char* nomeFicheiro) {
    FILE* f;
    f = fopen(nomeFicheiro, "r");
    if (f == NULL) {
        errorMessage("Ficheiro nao existe!");
        return 0;
    }
    fclose(f);
    return 1;
}

/**
 * Função responsável por verificar se já existe algum servidor em execução.
 * @return < 0 caso já exista, > 0 caso contrário
 */
int verifySingleInstance() {
    return open("/tmp/unique.txt", O_RDONLY | O_EXCL | O_CREAT);
}

/**
 * Função responsável por iniciar o Aspell.
 * @param fdWrite Descritor para escrita
 * @param fdRead Descritor para leitura
 */
void startAspell(int* fdWrite, int* fdRead) {
    int fdPipeToAspell[2];
    int fdPipeFromAspell[2];

    pipe(fdPipeToAspell);
    pipe(fdPipeFromAspell);

    printf("[SERVIDOR] Aspell vai ser iniciado!\n");

    if (fork() == 0) {
        dup2(fdPipeToAspell[0], STDIN_FILENO);
        close(fdPipeToAspell[1]);
        dup2(fdPipeFromAspell[1], STDOUT_FILENO);
        close(fdPipeFromAspell[0]);
        execlp("aspell", "aspell", "-a", "-d", "pt_PT", NULL);
    } else {
        close(fdPipeToAspell[0]);
        close(fdPipeFromAspell[1]);
        *fdWrite = fdPipeToAspell[1];
        *fdRead = fdPipeFromAspell[0];

        int bytesRead;
        char resp[4096];

        bytesRead = read(*fdRead, resp, 4096);
        resp[bytesRead - 1] = 0;
        //printf("Aspell: <%s>\n", resp);
    }
}

/**
 * Função responsável por contar palavras de um array de caractéres
 * @param msg mensagem
 * @return numero de palavras
 */
int contaPalavras(char * msg) {
    char tempMsg[1024];
    strncpy(tempMsg, msg, 1024);
    char* token;
    int conta = 0;

    token = strtok(tempMsg, " .,;:_?!");
    if (token == NULL)
        return conta;
    conta++;

    while ((token = strtok(NULL, " .,;:_?!")) != NULL)
        conta++;

    return conta;
}

/**
 * Função responsável por contar asteriscos numa mensagem.
 * @param msg mensagem
 * @return número de asteriscos
 */
int contaAsteriscos(char* msg) {
    int i = 0, conta = 0;

    while (msg[i] != '\0') {
        if (msg[i] == '*')
            conta++;
        i++;
    }

    return conta;
}

/**
 * Função responsável por enviar uma mensagem para o Aspell.
 * @param msg mensagem
 * @return 0 se a frase está correta, 1 caso contrário
 */
int spellCheckSentence(char * msg, int fdWrite, int fdRead) {
    int bytesRead;
    char resp[4096];
    char resp2[4096];
    char tempMsg[1024];
    strncpy(tempMsg, msg, 1024);
    char pal[51];
    char* token;

    token = strtok(tempMsg, " .,;:_?!");
    if (token == NULL)
        return 0;

    int errou = 0;

    do {
        strncpy(pal, token, 49);
        pal[50] = '\0';

        write(fdWrite, pal, strlen(pal));
        write(fdWrite, "\n", 1);
        //printf("Escrevi: <%s\n>", pal);

        bytesRead = read(fdRead, resp, 4095);
        if (resp[0] != '*')
            errou = 1;

        // Ler até encontrar 2 \n
        int count = 0;
        while (count < 2) {
            resp[bytesRead] = '\0';
            count += countNewLines(resp);
            if (count < 2)
                bytesRead = read(fdRead, resp, 4095);
        }

        if (errou)
            break;

    } while ((token = strtok(NULL, " .,;:_?!")) != NULL);

    return errou;
}

int countNewLines(char* buffer) {
    int count = 0;
    int i;

    for (i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == '\n') {
            count++;
        }
    }

    return count;
}

/**
 * 
 * @param line
 * @param tam
 * @return 1 se linha está vazia
 */
int isLineEmpty(char* line, int tam) {
    int i;
    for (i = 0; i < tam; i++) {
        if (line[i] != ' ')
            return 0;
    }
    return 1;
}

/**
 * 
 * @param a
 * @param size
 * @param c
 * @return 1 se existe, 0 caso contrário
 */
int doesCharExistInArray(char* a, int size, char c) {
    if (a == NULL)
        return 0;

    for (int i = 0; i < size; i++) {
        if (a[i] == c)
            return 1;
    }

    return 0;
}

/**
 * Esta função não conta espaços em branco
 * @param eData
 * @param numUniqueChars
 * @return 
 */
char* getArrayOfUniqueChars(EditorData eData, int* numUniqueChars) {
    char* array = NULL, *temp = NULL;
    *numUniqueChars = 0;

    for (int i = 0; i < eData.lin; i++) {
        for (int j = 0; j < eData.col; j++) {
            if (eData.lines[i].text[j] != ' ' && !doesCharExistInArray(array, *numUniqueChars, eData.lines[i].text[j])) {
                temp = realloc(array, sizeof (char) * (*numUniqueChars + 1));
                if (temp == NULL) {
                    errorMessage("Erro a allocar memória.");
                    return array;
                }
                array = temp;
                array[(*numUniqueChars)++] = eData.lines[i].text[j];
            }
        }
    }
    return array;
}

int countChars(EditorData eData, char c) {
    int count = 0;

    for (int i = 0; i < eData.lin; i++) {
        for (int j = 0; j < eData.col; j++) {
            if (eData.lines[i].text[j] == c)
                count++;
        }
    }
    return count;
}