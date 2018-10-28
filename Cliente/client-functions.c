#include "client-functions.h"
#include "client-defaults.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "client-defaults.h"
// OLD VERSION
/*
#define X_INDEX 5

void resetLine(char *text);
void preencheLinhas(int rows);
void moveUp(int *posy);
void moveDown(int *posy, int nrow);
void moveLeft(int *posx, int posy);
void moveRight(int *posx, int ncol);
void changeCursorPosition(int posx, int oposx, int posy, int oposy, char cursor, char tempChar, int ncol, int nav, int apaga);
void backSpaceKey(int posy, int posx, int ncol);
void deleteKey(int posy, int posx, int ncol);
int moveAllToTheRight(int posy, int posx, int ncol);
void getTextoDaLinha(char* texto, int linha, int maxX);
 */

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
void resetLine(WINDOW* w, int line, int dimX);
void writeTitle();
void refreshCursor(int y, int x);
void editMode(int y, int x, char* linha);
void writeKey(int key, char* linha, int x);
void getLinha(char* linha, int y);
void backSpaceKey(char* linha, int x, int y);
void deleteKey(char* linha, int x, int y);
//WINDOW* masterWin;
WINDOW* titleWin;
WINDOW* userWin;
WINDOW* lineWin;
WINDOW* editorWin;
Line lines[WIN_EDITOR_MAX_Y];

/**
 * Verifica se ao inicializar o programa do cliente foi introduzido algum argumento.
 * @param argc quantidade de argumentos
 * @param argv array com os argumentos
 */
void checkArgs(int argc, char** argv) {
    if (argc == 3) {
        char *cmd;
        int res;

        while ((res = getopt(argc, argv, "u:")) != -1) {
            switch (res) {
                case 'u':
                    cmd = optarg;
                    if (strlen(cmd) < 8)
                        editor(cmd);
                    else
                        loginSession();
                    //TODO Verificar se o utilizador existe do lado do servidor
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
    editor(user);
}

/**
 * Função responsável por tudo acerca do editor.
 */
void editor(char* user) { /*receber nome do utilizador e escreve-lo só em modo de edição*/
    initscr();
    start_color();
    clear();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

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
    //writeUser(user, 4);

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
    wrefresh(editorWin);

    int key, x = 0, y = 0;
    char linha[WIN_EDITOR_MAX_X];
    mvwprintw(stdscr, 19, 0, "Em modo de navegacao");
    refreshCursor(y, x);

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
            case 10:
                //TODO SE LINHA ESTÁ LIVRE, COLOCA OCUPADA E COMEÇA EDIÇÃO
                writeUser(user, y);
                getLinha(linha, y);
                editMode(y, x, linha);
                // TODO DESOCUPA A LINHA
                mvwprintw(stdscr, 19, 0, "Em modo de navegacao");
                break;
        }
        refreshCursor(y, x);
    }
    endwin();
    return;
}

void editMode(int y, int x, char* linha) {
    int key;
    char linhaTemp[WIN_EDITOR_MAX_X], resolverBug[WIN_EDITOR_MAX_X];
    getLinha(resolverBug, y + 1);
    strncpy(linhaTemp, linha, WIN_EDITOR_MAX_X);
    mvwprintw(stdscr, 19, 0, "Em modo de edicao   ");
    refreshCursor(y, x);
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
                break;
            case KEY_DOWN:
                break;
            case 10:
                resetLine(userWin, y, WIN_USER_MAX_X);
                return;
            case KEY_BACKSPACE:
                backSpaceKey(linha, x, y);
                if (x > 0)
                    x--;
                break;
            case 330:
                deleteKey(linha, x, y);
                break;
            default:
                writeKey(key, linha, x);
                resetLine(editorWin, y, WIN_EDITOR_MAX_X);
                writeTextLine(linha, y);
                break;
        }
        if (key != 27)
            refreshCursor(y, x);
    }
    resetLine(userWin, y, WIN_USER_MAX_X);
    writeTextLine(linhaTemp, y);
}

WINDOW * createSubWindow(WINDOW* janelaMae, int dimY, int dimX, int startY, int startX) {
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
    wrefresh(userWin);
}

void writeDocument(char* text[], int nLines) {
    int i;
    for (i = 0; i < nLines; i++)
        writeTextLine(text[i], i);
}

void writeTextLine(char* text, int line) {
    mvwprintw(editorWin, line, 0, "%s", text);
    wrefresh(editorWin);
}

void clearEditor(int dimY, int dimX) {
    int i;
    for (i = 0; i < dimY; i++) {
        resetLine(editorWin, i, dimX);
    }
}

void resetLine(WINDOW* w, int line, int dimX) {
    for (int i = 0; i < dimX; i++)
        mvwprintw(w, line, i, " ");
    wrefresh(w);
}

void refreshCursor(int y, int x) {
    int cy, cx;
    wmove(editorWin, y, x);
    getyx(editorWin, cy, cx);
    mvwprintw(stdscr, 20, 0, "l: %02d\tc: %02d", cy, cx);
    wrefresh(stdscr);
    wrefresh(editorWin);
}

int moveAllToTheRight(char* linha, int x) {
    int max = WIN_EDITOR_MAX_X - 1;
    if (linha[max] != ' ')
        return 0;
    for (; max > x; max--)
        linha[max] = linha[max - 1];
    return 1;
}

void writeKey(int key, char* linha, int x) {
    if (moveAllToTheRight(linha, x))
        linha[x] = key;
}

void getLinha(char* linha, int y) {
    int i;
    for (i = 0; i < WIN_EDITOR_MAX_X; i++)
        linha[i] = mvwinch(editorWin, y, i) & A_CHARTEXT; //para extrair o caracter; wvminch devolve um chtype e não um char
}

void moveAllToTheLeft(char* linha, int x) {
    int max = WIN_EDITOR_MAX_X - 1;
    for (; x < max; x++)
        linha[x] = linha[x + 1];
    linha[WIN_EDITOR_MAX_X - 1] = ' ';
}

void backSpaceKey(char* linha, int x, int y) {
    if (x > 0) {
        moveAllToTheLeft(linha, x - 1);
        resetLine(editorWin, y, WIN_EDITOR_MAX_X);
        writeTextLine(linha, y);
    }
}

void deleteKey(char* linha, int x, int y) {
    moveAllToTheLeft(linha, x);
    resetLine(editorWin, y, WIN_EDITOR_MAX_X);
    writeTextLine(linha, y);
}










































/* OLD VERSION OF THE WORK */

/**
 * Função responsável por tudo acerca do editor.
 */
/*void editor() {
    int nrow, ncol, posx, posy, oposx, oposy;
    int ch;
    char cursor = 219, tempChar;
    Line linha;
    linha.free = 1; // TODO Isto terá que sair daqui posteriormente
    resetLine(linha.text); // TODO Isto terá que sair daqui posteriormente

    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, nrow, ncol);
    curs_set(0);
    preencheLinhas(nrow);
    posy = nrow / 2;
    posx = X_INDEX;
    mvprintw(0, 0, "Modo de Navegacao");
    mvprintw(0, ncol - 20, "Cursor: (%d, %d)  ", posy, posx);
    mvaddch(posy, posx, cursor);
    refresh();
    do {
        ch = getch();
        oposy = posy;
        oposx = posx;
        switch (ch) {
            case KEY_UP:
                tempChar = mvinch(posy - 1, posx);
                moveUp(&posy);
                break;
            case KEY_DOWN:
                tempChar = mvinch(posy + 1, posx);
                moveDown(&posy, nrow);
                break;
            case KEY_LEFT:
                tempChar = mvinch(posy, posx + 1);
                moveLeft(&posx, posy);
                break;
            case KEY_RIGHT:
                tempChar = mvinch(posy, posx - 1);
                moveRight(&posx, ncol);
                break;
            case 10:
                if (linha.free) {
                    linha.free = 0;
                    editMode(linha.text, posx, posy, ncol, cursor);
                    getTextoDaLinha(linha.text, posy, ncol);
                    linha.free = 1;
                }
                break;
            case 27: // ESC
                endwin();
                printf("\n%s\n", linha.text);
                return;
        }
        if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT)
            changeCursorPosition(posx, oposx, posy, oposy, cursor, tempChar, ncol, 1, 0);
    } while (posy != (nrow - 1) || posx != (ncol - 1));
    endwin();
}

/**
 * Função denominada como modo de edição. Esta é a função que faz com que o
 * utilizador possa escrever, apagar, gravar, descartar alterações, entre outras
 * funcionalidades.
 * @param text String que estava na linha antes de começar a ser editada.
 * @param posx Posição atual no eixo dos X.
 * @param posy Posição atual no eixo dos Y.
 * @param ncol Número de colunas.
 * @param cursor Caractér que representa o cursor.
 */
/*void editMode(char* text, int posx, int posy, int ncol, char cursor) {
    int ch, oposx, apaga = 0;
    char temp[DEFAULT_MAXCOLUMNS], tempChar;
    mvprintw(0, 0, "Modo de Edicao   ");
    refresh();
    strncpy(temp, text, DEFAULT_MAXCOLUMNS);
    do {
        apaga = 0;
        ch = getch();
        oposx = posx;
        switch (ch) {
            case KEY_UP:
                break;
            case KEY_DOWN:
                break;
            case KEY_LEFT:
                tempChar = mvinch(posy, posx - 1);
                moveLeft(&posx, posy);
                break;
            case KEY_RIGHT:
                tempChar = mvinch(posy, posx + 1);
                moveRight(&posx, ncol);
                break;
            case 10: // Enter
                mvprintw(0, 0, "Modo de Navegacao");
                refresh();
                return;
            case KEY_BACKSPACE:
                apaga = 1;
                backSpaceKey(posy, posx, ncol);
                moveLeft(&posx, posy);
                break;
            case 27: // ESC
                mvprintw(posy, X_INDEX, temp);
                mvprintw(0, 0, "Modo de Navegacao");
                refresh();
                return;
            case 127:
                deleteKey(posy, posx, ncol);
                break;
            default:
                if (moveAllToTheRight(posy, posx, ncol)) {
                    mvaddch(posy, posx, ch);
                    tempChar = ch;
                    moveRight(&posx, ncol);
                }
                break;
        }
        if (ch != 10 && ch != 27)
            changeCursorPosition(posx, oposx, posy, posy, cursor, tempChar, ncol, 0, apaga);
    } while (posx != (ncol - 1));
}

/**
 * Esta função inicializa as linhas.
 * @param text string para ser inicializada
 */
/*void resetLine(char *text) {
    for (int i = 0; i < 45; i++)
        text[i] = ' ';
}

/**
 * Esta função enumera as linhas do editor.
 * @param rows quantidade de linhas a enumerar.
 */
/*void preencheLinhas(int rows) {
    for (int i = 1; i <= rows; i++) {
        mvprintw(i, 0, "%02d - ", i - 1);
    }
}

void moveUp(int *posy) {
 *posy = (*posy > 0) ? *posy - 1 : *posy;
}

void moveDown(int *posy, int nrow) {
 *posy = (*posy < (nrow - 1)) ? *posy + 1 : *posy;
}

void moveLeft(int *posx, int posy) {
 *posx = (*posx > 0) ? *posx - 1 : posy;
}

void moveRight(int *posx, int ncol) {
 *posx = (*posx < (ncol - 1)) ? *posx + 1 : *posx;
}

/**
 * Esta função é responsável por atualizar o cabeçalho (modo ativo / localização
 * do cursor). Também verifica os limites e atualiza o cursor, mantendo o
 * caractér que estava anteriormente no seu respectivo lugar.
 * @param posx Posição atual no eixo dos X.
 * @param oposx Posição anterior no eixo dos X.
 * @param posy Posição atual no eixo dos Y.
 * @param oposy Posição anterior no eixo dos Y.
 * @param cursor Caractér que representa o cursor.
 * @param tempChar Caractér que estava antes do cursor ficar por cima.
 * @param ncol Número de colunas.
 * @param nav 1 caso esteja em modo navegação e 0 caso contrário.
 */
/*void changeCursorPosition(int posx, int oposx, int posy, int oposy, char cursor, char tempChar, int ncol, int nav, int apaga) {
    if (!nav) mvprintw(0, 0, "Modo de Edicao   ");
    else mvprintw(0, 0, "Modo de Navegacao");
    mvprintw(0, ncol - 20, "Cursor: (%d, %d)  ", posy, posx);
    if (tempChar == cursor) tempChar = ' ';
    if (posx < 5) posx = 5;
    if (posy < 1) posy = 1;
    if (!apaga)
        mvaddch(oposy, oposx, tempChar);
    mvaddch(posy, posx, cursor);
    refresh();
}

/**
 * Função responsável por fazer tudo o efeito que é suposto a tecla backspace
 * fazer num editor já criado.
 * @param posy Posição atual no eixo dos Y.
 * @param posx Posição atual no eixo dos X.
 * @param ncol Número de colunas.
 */
/*void backSpaceKey(int posy, int posx, int ncol) { //TODO CONSERTAR BUG: Está a manter o caracter onde tava o cursor!
    char tempChar;
    for (; posx < ncol - 1; posx++) {
        tempChar = mvinch(posy, posx + 1);
        mvaddch(posy, posx, tempChar);
    }
    mvaddch(posy, posx + 1, ' ');
    refresh();
}

/**
 * Função responsável por fazer tudo o efeito que é suposto a tecla delete
 * fazer num editor já criado.
 * @param posy Posição atual no eixo dos Y.
 * @param posx Posição atual no eixo dos X.
 * @param ncol Número de colunas.
 */
/*void deleteKey(int posy, int posx, int ncol) { //TODO CONSERTAR BUG: Não está a funcionar como devia
    char tempChar;
    mvaddch(posy, ncol, ' ');
    for (; ncol > posx; ncol--) {
        tempChar = mvinch(posy, ncol);
        mvaddch(posy, ncol, ' ');
        mvaddch(posy, ncol - 1, tempChar);
    }
    refresh();
}

/**
 * Função responsável por puxar os caracteres todos para a direita, para que seja
 * possivel escrever entre palavras.
 * @param posy Posição atual no eixo dos Y.
 * @param posx Posição atual no eixo dos X.
 * @param ncol Número de colunas.
 * @return 1 se foi possivel, 0 caso contrário.
 */
/*int moveAllToTheRight(int posy, int posx, int ncol) {
    if (mvinch(posy, ncol - 1) != ' ')
        return 0;
    char tempChar;
    for (; ncol > posx; ncol--) {
        tempChar = mvinch(posy, ncol - 1);
        mvaddch(posy, ncol, tempChar);
    }
    refresh();
    return 1;
}

void getTextoDaLinha(char* texto, int linha, int maxX) {
    for (int i = 0; i < maxX; i++)
        texto[i] = mvinch(linha, i + X_INDEX);
}
 */