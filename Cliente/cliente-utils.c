/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   cliente-utils.c
 * Author: ricardo
 * 
 * Created on October 18, 2018, 3:46 PM
 */

#include "cliente-utils.h"
#include <ncurses.h>
#include <string.h>
#define CH '*'

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

    initscr();
    clear();
    noecho();
    cdbreak();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, nrow, ncol);
    posy = nrow / 2;
    posx = ncol / 2;
    mvaddch(posy, posx, C);
    mvprintw(0, 0, "(%d,%d)  ", posy, posx);
    refresh();
    do {
        ch = getch();
        oposy = posy;
        oposx = posx;
        switch (ch) {
            case KEY_UP:
                posy = (posy > 0) ? posy - 1 : posy;
                break;
            case KEY_DOWN:
                //posy = (posy < ()) ? posy - 1 : posy;
                break;
            case KEY_UP:
                posy = (posy > 0) ? posy - 1 : posy;
                break;
            case KEY_UP:
                posy = (posy > 0) ? posy - 1 : posy;
                break;
        }
    } while (posy != (nrow.1) || posx != (ncol - 1));
}