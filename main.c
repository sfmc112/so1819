#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#define TITLE "MEDIT EDITOR--------------------filename.xpto-----------------------------------"

#define WIN_EDITOR_MAX_X 45
#define WIN_EDITOR_MAX_Y 15

#define WIN_USER_MAX_X 8
#define WIN_USER_MAX_Y WIN_EDITOR_MAX_Y

#define WIN_LINENUM_MAX_X 2
#define WIN_LINENUM_MAX_Y WIN_EDITOR_MAX_Y

#define WIN_TITLE_MAX_X COLS
#define WIN_TITLE_MAX_Y 1

WINDOW* createSubWindow(WINDOW* janelaMae, int dimY, int dimX, int startY, int startX);
void configureWindow(WINDOW* janela, int setCores);
void writeLineNumbers();
void writeUser(char* name, int line);
void writeDocument(char* text[], int nLines);
void writeTextLine(char* text, int line);
void clearEditor(int dimY, int dimX);
void resetLine(int line, int dimX);
void writeTitle();

//WINDOW* masterWin;
WINDOW* titleWin;
WINDOW* userWin;
WINDOW* lineWin;
WINDOW* editorWin;

/*
 * 
 */
int main(int argc, char** argv) {

    initscr();
    start_color();
    clear();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    //curs_set(0);

    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);

    titleWin = createSubWindow(stdscr, WIN_TITLE_MAX_Y, WIN_TITLE_MAX_X, 0, 0);
    userWin = createSubWindow(stdscr, WIN_USER_MAX_Y, WIN_USER_MAX_X, WIN_TITLE_MAX_Y + 1, 0);
    lineWin = createSubWindow(stdscr, WIN_LINENUM_MAX_Y, WIN_LINENUM_MAX_X, WIN_TITLE_MAX_Y + 1, WIN_USER_MAX_X + 1);
    editorWin = createSubWindow(stdscr, WIN_EDITOR_MAX_Y, WIN_EDITOR_MAX_X, WIN_TITLE_MAX_Y + 1, WIN_LINENUM_MAX_X + WIN_USER_MAX_X + 2);


    configureWindow(userWin, COLOR_PAIR(3));
    configureWindow(lineWin, COLOR_PAIR(3));
    configureWindow(editorWin, COLOR_PAIR(2));
    configureWindow(titleWin, COLOR_PAIR(1));

    writeTitle();
    writeLineNumbers();
    writeUser("RicardoB", 4);



    wmove(editorWin, 0, 0);
    wrefresh(stdscr);
    wrefresh(editorWin);




    char* doc[WIN_EDITOR_MAX_Y] = {
        "Ola tudo fixe isto e o documento brutal",
        "vai ter duas linhas e ja e bem bom",
        "oops afinal tem mais, isto e so um teste!",
        " "
    };

    clearEditor(WIN_EDITOR_MAX_Y, WIN_EDITOR_MAX_X);
    writeDocument(doc, 3);

    //wmove(editorWin, 0, 0);
    wrefresh(editorWin);


    //mvwgetch(editorWin, 0, 0);
    //wgetch(editorWin);

    int key;
    int x = 0, y = 0;
    /*
        getsyx(y,x);
        wmove(editorWin, y, x);
        wrefresh(editorWin);
     */
    int cx, cy;

    while ((key = getch()) != 27) {
        switch (key) {
            case KEY_LEFT:
                if (x > 0)
                    x--;
                break;

            case KEY_RIGHT:
                if (x < WIN_EDITOR_MAX_X)
                    x++;
                break;

            case KEY_UP:
                if (y > 0)
                    y--;
                break;

            case KEY_DOWN:
                if (y < WIN_EDITOR_MAX_Y)
                    y++;
                break;
        }

        wmove(editorWin, y, x);
        getyx(editorWin, cy, cx);
        mvwprintw(stdscr, 20, 0, "l: %02d\tc: %02d", cy, cx);
        wmove(editorWin, y, x);
        wrefresh(stdscr);
        wrefresh(editorWin);
    }


    endwin();
    printf("%d\n", key);

    return (EXIT_SUCCESS);
}

WINDOW* createSubWindow(WINDOW* janelaMae, int dimY, int dimX, int startY, int startX) {
    return subwin(janelaMae, dimY, dimX, startY, startX);
}

void configureWindow(WINDOW* janela, int setCores) {
    wattrset(janela, setCores);
    wbkgd(janela, setCores);
    wclear(janela);
    wrefresh(janela);
}

void writeTitle() {
    mvwprintw(titleWin, 0, 0, "%s", TITLE);
}

void writeLineNumbers() {
    int i;

    for (i = 0; i < WIN_LINENUM_MAX_Y; i++) {
        mvwprintw(lineWin, i, 0, "%02d", i);
    }
}

void writeUser(char* name, int line) {
    mvwprintw(userWin, line, 0, "%s", name);
}

void writeDocument(char* text[], int nLines) {
    int i;
    for (i = 0; i < nLines; i++)
        writeTextLine(text[i], i);
}

void writeTextLine(char* text, int line) {
    mvwprintw(editorWin, line, 0, "%s", text);
}

void clearEditor(int dimY, int dimX) {
    int i;
    for (i = 0; i < dimY; i++) {
        resetLine(i, dimX);
    }
}

void resetLine(int line, int dimX) {
    for (int i = 0; i < dimX; i++)
        mvwprintw(editorWin, line, i, " ");
}

