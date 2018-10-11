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

int cmdShutdown(){
    puts("Comando shutdown nao implementado!");
    return 0;
}

int cmdSettings(){
    puts("Comando settings nao implementado!");
    return 0;
}

int cmdLoad(){
    puts("Comando load nao implementado!");
    return 0;
}

int cmdSave(){
    puts("Comando save nao implementado!");
    return 0;
}

int cmdFree(){
    puts("Comando free nao implementado!");
    return 0;
}

int cmdStats(){
    puts("Comando statistics nao implementado!");
    return 0;
}

int cmdUsers(){
    puts("Comando users nao implementado!");
    return 0;
}

int cmdText(){
    puts("Comando text nao implementado!");
    return 0;
}
/**
 * Função responsável por ler o comando e interpreta-o.
 * @return 1 se conseguiu e 0 caso contrário
 */
int lerComandos() {
    char comando[20]; //TODO alterar para define
    const char* listaComandos[] = {"shutdown", "settings", "load", "save", "free", "statistics", "users", "text"};
    char* token = NULL;
    setbuf(stdout, NULL);
    int i;
    while (1) {
        printf("Introduza o comando: ");
        scanf(" %19[^\n]", comando); //TODO temos que resolver o problema de inserir caracteres a mais! Nao esta a funcionar
        //comando tudo em letras minusculas
        toLower(comando);
        //1ª parte do comando
        token = strtok(comando, " ");
        //Imprime comando
        printf("%s\n", token);
        for (i = 0; i < 8 && strcmp(listaComandos[i], token) != 0; i++)
            ;
        switch (i) {
            case 0:
                cmdShutdown();
                break;
            case 1:
                cmdSettings();
                break;

            case 2:
                cmdLoad();
                break;
            case 3:
                cmdSave();
                break;
            case 4:
                cmdFree();
                break;
            case 5:
                cmdStats();
                break;
            case 6:
                cmdUsers();
                break;
            case 7:
                cmdText();
                break;
            default:
                puts("Comando invalido!");
                
        }
    }
}