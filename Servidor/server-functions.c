/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   server-functions.c
 * Author: ricardo
 * 
 * Created on October 18, 2018, 11:31 AM
 */

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
 * Função para verificar argumentos de inicialização do programa.
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
                    if(ifFileExists(cmd))
                        strncpy(sd->usersDB, cmd, MAXSIZE);
                    else
                        strncpy(sd->usersDB,USERSDEFAULT_DB ,MAXSIZE);
                    break;
            }
        }
    }else{
        strncpy(sd->usersDB,USERSDEFAULT_DB ,MAXSIZE);
    }
}