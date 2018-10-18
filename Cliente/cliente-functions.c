/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   cliente-functions.c
 * Author: ricardo
 * 
 * Created on October 18, 2018, 3:41 PM
 */

#include "cliente-functions.h"
#include "cliente-utils.h"

void checkArgs(int argc, char** argv) {
    if (argc == 3) {
        char* cmd;
        int res;

        while ((res = getopt(argc, argv, "u")) != -1) {
            switch (res) {
                case 'u':
                    cmd = optarg;
                    //TODO Verificar se o utilizador existe do lado do servidor
                    editor();
                    break;
            }
        }
    }else{
        loginSession();
    }
}