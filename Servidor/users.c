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
#define USERSDEFAULT_DB "medit.db"

/**
 * Função para validar username.
 * @param user nome de utilizador
 * @return 1 se valido e 0 caso contrário
 */
int checkUsername(char* user) {
    if (strlen(user) <= 8) {
        FILE* f;
        char buffer[10];
        f = fopen(USERSDEFAULT_DB, "r");
        if (f == NULL)
            exitError("Nao consegui abrir o ficheiro!");
        while (fscanf(f, " %[^\n]", buffer) == 1)
            if (strcmp(buffer, user) == 0) {
                fclose(f);
                return 1;
            }
        fclose(f);
    }   
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
    if (checkUsername(user)) {
        puts("Esse username ja esta registado na nossa base de dados.");
        return 0;
    } else {
        FILE* f;
        f = fopen(USERSDEFAULT_DB, "a");
        if (f == NULL)
            exitError("Nao consegui abrir o ficheiro!");
        fprintf(f, "%s\n", user);
        fclose(f);
        return 1;
    }
}