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
#include "client-defaults.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

void printText(char *text);
void resetLine(char *text);

/**
 * Verifica se ao inicializar o programa do cliente foi introduzido algum argumento.
 * @param argc quantidade de argumentos
 * @param argv array com os argumentos
 */
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
    } else {
        loginSession();
    }
}

/**
 * Função que pede o nome de utilizador e pede para que o servidor verifque se existe, fazendo assim o login ou não.
 */
void loginSession() {
    char user[9];
    printf("Insira o nome de utilizador: ");
    scanf(" %8s", user);
    //TODO Verificar se o utilizador existe do lado do servidor
    editor();
}

void editor() {
    int nrow, ncol, posx, posy, oposx, oposy;
    int ch;
    char cursor = 219;
    Line linha;
    resetLine(linha.text);

    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, nrow, ncol);
    posy = nrow / 2;
    posx = ncol / 2;
    mvaddch(posy, posx, cursor);
    mvprintw(0, 0, "(%d,%d)  ", posy, posx);
    refresh();
    do {
        printText(linha.text);
        ch = getch();
        oposy = posy;
        oposx = posx;
        switch (ch) {
            case KEY_UP:
                //posy = (posy > 0) ? posy - 1 : posy;
                break;
            case KEY_DOWN:
                //posy = (posy < (nrow-1)) ? posy + 1 : posy;
                break;
            case KEY_LEFT:
                posx = (posx > 0) ? posx - 1 : posy;
                break;
            case KEY_RIGHT:
                posx = (posx < (ncol - 1)) ? posx + 1 : posx;
                break;
            case 10:
                if (linha.free)
                    editMode(linha.text);
                break;
        }

        if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT || ch == 10) {
            mvaddch(oposy, oposx, ' ');
            mvaddch(posy, posx, cursor);
            mvprintw(0, 0, "(%d,%d)   ", posy, posx);
            refresh();
        }
    } while (posy != (nrow - 1) || posx != (ncol - 1));

    endwin();
}

void editMode(char* text, int posx, int posy) {
    //mvprintw(0, 0, "              Modo de edicao: ON\n");
    //TODO--------
}

void printText(char *text) {
    int i, j;
    for (i = 0, j = 5; i < 45; i++, j++) {
        mvprintw(j, 8, "%c", text[i]);
    }

}

void resetLine(char *text) {
    //Inicializar linhas
    int j;
    for (j = 0; j < 45; j++)
        text[j] = ' ';
}