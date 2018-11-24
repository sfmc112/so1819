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
#define KEY_ESC 27
#define KEY_ENTR 10
#define KEY_DELETE 330

WINDOW* createSubWindow(WINDOW* janelaMae, int dimY, int dimX, int startY, int startX);
void configureWindow(WINDOW* janela, int setCores);
void writeLineNumbers();
void writeUser(char* name, int line);
void writeDocument(Line *text, int nLines);
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

        while ((res = getopt(argc, argv, "u:p:")) != -1) {
            switch (res) {
                case 'u':
                    cmd = optarg;
                    if (strlen(cmd) <= 8) {
                        //TODO Verificar se o utilizador existe do lado do servidor
                        editor(cmd);
                    } else
                        loginSession();
                    break;
                case 'p':
                    cmd = optarg;
                    // TODO CRIAR PIPE
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
        "oops afinal tem mais, isto e so um teste!"
    };
    for (int i = 0; i < WIN_EDITOR_MAX_Y; i++) {
        lines[i].free = 1;
        for (int j = 0; j < WIN_EDITOR_MAX_X; j++) {
            lines[i].text[j] = ' ';
        }

    }

    clearEditor(WIN_EDITOR_MAX_Y, WIN_EDITOR_MAX_X);
    writeDocument(lines, WIN_EDITOR_MAX_Y);
    wrefresh(editorWin);

    int key, x = 0, y = 0;
    //char linha[WIN_EDITOR_MAX_X];
    mvwprintw(stdscr, 19, 0, "Em modo de navegacao");
    refreshCursor(y, x);

    while ((key = getch()) != KEY_ESC) {
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
            case KEY_ENTR:
                //TODO SE LINHA ESTÁ LIVRE, COLOCA OCUPADA E COMEÇA EDIÇÃO
                writeUser(user, y);
                //getLinha(linha, y);
                lines[y].free = 0;
                editMode(y, x, lines[y].text);
                writeDocument(lines, WIN_EDITOR_MAX_Y);
                lines[y].free = 1;
                // TODO DESOCUPA A LINHA
                mvwprintw(stdscr, 19, 0, "Em modo de navegacao");
                break;
        }
        refreshCursor(y, x);
    }
    endwin();
    return;
}

/**
 * Esta função é responsável por gerir a edição de linha.
 * @param y Posição Y do cursor
 * @param x Posição X do cursor
 * @param linha linha de texto
 */
void editMode(int y, int x, char* linha) {
    int key;
    char linhaTemp[WIN_EDITOR_MAX_X];
    strncpy(linhaTemp, linha, WIN_EDITOR_MAX_X);
    mvwprintw(stdscr, 19, 0, "Em modo de edicao   ");
    refreshCursor(y, x);
    while ((key = getch()) != KEY_ESC) {
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
            case KEY_ENTR:
                resetLine(userWin, y, WIN_USER_MAX_X);
                return;
            case KEY_BACKSPACE:
                backSpaceKey(linha, x, y);
                if (x > 0)
                    x--;
                break;
            case KEY_DELETE:
                deleteKey(linha, x, y);
                break;
            default:
                writeKey(key, linha, x);
                resetLine(editorWin, y, WIN_EDITOR_MAX_X);
                writeTextLine(linha, y);
                if (x < WIN_EDITOR_MAX_X - 1)
                    x++;
                break;
        }
        if (key != KEY_ESC)
            refreshCursor(y, x);
    }
    resetLine(userWin, y, WIN_USER_MAX_X);
    strncpy(linha, linhaTemp, WIN_EDITOR_MAX_X);
}

/**
 * Esta função é responsável pela criação de uma sub janela.
 * @param janelaMae Contexto onde vai ser criada a nova janela.
 * @param dimY Dimensão total em altura.
 * @param dimX Dimensão total em largura.
 * @param startY Linha inicial.
 * @param startX Coluna inicial.
 * @return Nova subjanela da janelaMae.
 */
WINDOW * createSubWindow(WINDOW* janelaMae, int dimY, int dimX, int startY, int startX) {
    return subwin(janelaMae, dimY, dimX, startY, startX);
}

/**
 * Esta função é responsável pela configuração da janela.
 * @param janela
 * @param setCores cores
 */
void configureWindow(WINDOW* janela, int setCores) {
    wattrset(janela, setCores);
    wbkgd(janela, setCores);
    wclear(janela);
    wrefresh(janela);
}

/**
 * Função responsável por escrever na janela titleWin um título.
 */
void writeTitle() {
    mvwprintw(titleWin, 0, 0, "%s", TITLE);
}

/**
 * Função responsável por escrever a identicação numerada de cada linha.
 */
void writeLineNumbers() {
    for (int i = 0; i < WIN_LINENUM_MAX_Y; i++) {
        mvwprintw(lineWin, i, 0, "%02d", i);
    }
}

/**
 * Função responsável por escrever o nome do utilizador que está a editar
 * a linha.
 * @param name Nome de utilizador
 * @param line Linha
 */
void writeUser(char* name, int line) {
    mvwprintw(userWin, line, 0, "%s", name);
    wrefresh(userWin);
}

/**
 * Função responsável por escrever um array de linhas no ecrã.
 * @param text array de linhas
 * @param nLines número de linhas
 */
void writeDocument(Line *text, int nLines) {
    for (int i = 0; i < nLines; i++)
        writeTextLine(text[i].text, i);
}

/**
 * Função responsável por escrever um array de linhas no ecrã.
 * @param text String a ser escrita
 * @param line número da linha onde escrever
 */
void writeTextLine(char* text, int line) {
    mvwprintw(editorWin, line, 0, "%s", text);
    wrefresh(editorWin);
}

/**
 * Função responsável por limpar o editor entre duas dimensões.
 * @param dimY Altura
 * @param dimX Largura
 */
void clearEditor(int dimY, int dimX) {
    for (int i = 0; i < dimY; i++) {
        resetLine(editorWin, i, dimX);
    }
}

/**
 * Função responsável por escrever uma linha em branco numa determinada janela.
 * @param w Janela
 * @param line Linha
 * @param dimX Largura
 */
void resetLine(WINDOW* w, int line, int dimX) {
    for (int i = 0; i < dimX; i++)
        mvwprintw(w, line, i, " ");
    wrefresh(w);
}

/**
 * Função responsável por atualizar o cursor e as suas respectivas coordenadas.
 * @param y linha
 * @param x coluna
 */
void refreshCursor(int y, int x) {
    int cy, cx;
    wmove(editorWin, y, x);
    getyx(editorWin, cy, cx);
    mvwprintw(stdscr, 20, 0, "l: %02d\tc: %02d", cy, cx);
    wrefresh(stdscr);
    wrefresh(editorWin);
}

/**
 * Função é responsável por mover todo o texto a partir de uma posição X para a
 * direita.
 * @param linha linha de texto
 * @param x coluna
 * @return 0 se falhou, 1 caso contrário
 */
int moveAllToTheRight(char* linha, int x) {
    int max = WIN_EDITOR_MAX_X - 1;
    if (linha[max] != ' ')
        return 0;
    for (; max > x; max--)
        linha[max] = linha[max - 1];
    return 1;
}

/**
 * Função responsável por escrever uma tecla numa posição do array.
 * @param key Tecla
 * @param linha linha de texto
 * @param x posição no array
 */
void writeKey(int key, char* linha, int x) {
    if (moveAllToTheRight(linha, x))
        linha[x] = key;
}

/**
 * Função responsável por extrair a informação da janela do editor para o array.
 * @param linha linha de texto
 * @param y linha
 */
void getLinha(char* linha, int y) {
    for (int i = 0; i < WIN_EDITOR_MAX_X; i++)
        linha[i] = mvwinch(editorWin, y, i) & A_CHARTEXT; //para extrair o caracter; wvminch devolve um chtype e não um char
}

/**
 * Função responsável por mover todos os caractéres para a esquerda.
 * @param linha linha de texto
 * @param x posição no array
 */
void moveAllToTheLeft(char* linha, int x) {
    int max = WIN_EDITOR_MAX_X - 1;
    for (; x < max; x++)
        linha[x] = linha[x + 1];
    linha[WIN_EDITOR_MAX_X - 1] = ' ';
}

/**
 * Função responsável por efetuar o comportamento normal de um backspace,
 * ou seja, apagar caractér por caractér.
 * @param linha linha de texto
 * @param x posição no array
 * @param y linha
 */
void backSpaceKey(char* linha, int x, int y) {
    if (x > 0) {
        moveAllToTheLeft(linha, x - 1);
        resetLine(editorWin, y, WIN_EDITOR_MAX_X);
        writeTextLine(linha, y);
    }
}

/**
 * Função responsável por efetuar o comportamento normal de um delete, ou seja,
 * apagar caractér por caractér.
 * @param linha linha de texto
 * @param x posição no array
 * @param y linha
 */
void deleteKey(char* linha, int x, int y) {
    moveAllToTheLeft(linha, x);
    resetLine(editorWin, y, WIN_EDITOR_MAX_X);
    writeTextLine(linha, y);
}