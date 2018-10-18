/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   utils.c
 * Author: ricardo
 * 
 * Created on October 11, 2018, 12:03 PM
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Função que termina a aplicação com erro.
 * @param mensagem mensagem de erro
 */
void exitError(char* mensagem) {
    perror(mensagem);
    exit(1);
}

/**
 * Função para colocar toda a string em letras minusculas.
 * @param buffer string a ser transformada
 * @return 
 */
void toLower(char* buffer) {
    for (int i = 0; buffer[i] != '\0'; i++)
        buffer[i] = tolower(buffer[i]);
}

int ifFileExists(char* nomeFicheiro) {
    FILE* f;
    f = fopen(nomeFicheiro, "r");
    if (f == NULL)
        return 0;
    fclose(f);
    return 1;
}
