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
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

/**
 * Função para verificar argumentos.
 */
void checkArgs(int argc, char** argv, ServerData sd) {
    if (argc == 3) {
        char* cmd;
        int res;

        while ((res = getopt(argc, argv, "f")) != -1) {
            switch (res) {
                case 'f':
                    cmd = optarg;
                    strcpy_s(sd.usersDB,MAXSIZE,cmd);
                    break;
            }
        }
    }
}