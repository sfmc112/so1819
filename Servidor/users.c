/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   users.c
 * Author: ricardo
 * 
 * Created on October 11, 2018, 12:00 PM
 */

#include "users.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#define usersDB "users.db"
/**
 * Função para validar username.
 * @param user nome de utilizador
 * @return 1 se valido e 0 caso contrário
 */
int validaUsername(char* user) {
    FILE* f;
    char buffer[10];
    f = fopen(usersDB, "r");
    if (f == NULL)
        sairComErro("Nao consegui abrir o ficheiro!");
    while (fscanf(f, " %[^\n]", buffer) == 1)
        if (strncmp(buffer, user, sizeof (buffer) - 1) == 0) {
            fclose(f);
            return 1;
        }
    fclose(f);
    return 0;
}

/**
 * Função para adicionar username.
 * @return 1 se adicionou e 0 caso contrário
 */
int addUsername() {
    char user[10];
    printf("Indique o seu username: ");
    scanf(" %9[^\n]", user);
    if (validaUsername(user)) {
        puts("Esse username ja esta registado na nossa base de dados.");
        return 0;
    } else {
        FILE* f;
        f = fopen(usersDB, "a");
        if (f == NULL)
            sairComErro("Nao consegui abrir o ficheiro!");
        fprintf(f, "%s\n", user);
        fclose(f);
        return 1;
    }
}