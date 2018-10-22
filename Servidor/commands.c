/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   commands.c
 * Author: ricardo
 * 
 * Created on October 11, 2018, 12:08 PM
 */

#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "server-defaults.h"

void cmdShutdown() {
    exitNormal();
}

void cmdSettings(ServerData sd, EditorData ed) {
    printf("\n\n-----SETTINGS-----\n");
    printf("Numero de linhas do editor: %d\n", ed.lin);
    printf("Numero de colunas do editor: %d\n", ed.col);
    printf("Nome da base de dados de usernames: %s\n", sd.usersDB);
    printf("Numero maximo de utilizadores ativos: %d\n\n", sd.maxUsers);
}

int cmdLoad() {
    puts("Comando load nao implementado!");
    return 0;
}

int cmdSave() {
    puts("Comando save nao implementado!");
    return 0;
}

int cmdFree() {
    puts("Comando free nao implementado!");
    return 0;
}

int cmdStats() {
    puts("Comando statistics nao implementado!");
    return 0;
}

int cmdUsers() {
    puts("Comando users nao implementado!");
    return 0;
}

int cmdText() {
    puts("Comando text nao implementado!");
    return 0;
}
/**
 * Função para verificar se um determinado comando possui um argumento.
 * @param token comando
 * @return 1 se existe argumento, 0 se não.
 */
int checkCommandArgs(char* token) {
    token = strtok(NULL, " ");
    if (token != NULL)
        return 1;
    puts("Falta o segundo parametro");
    return 0;
}

