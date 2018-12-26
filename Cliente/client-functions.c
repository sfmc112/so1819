#include "client-functions.h"
#include "client-defaults.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/select.h>
#include <stdlib.h>
#include <pthread.h>
#include "client-defaults.h"

#define WIN_USER_MAX_X 8
#define WIN_LINENUM_MAX_X 2
#define WIN_TITLE_MAX_X COLS
#define WIN_TITLE_MAX_Y 1
#define KEY_ESC 27
#define KEY_ENTR 10
#define KEY_DELETE 330

WINDOW* createSubWindow(WINDOW* janelaMae, int dimY, int dimX, int startY, int startX);
void configureWindow(WINDOW* janela, int setCores);
void writeOneLineNumber(int i);
void writeLineNumbers(int lines);
void writeTextLine(char* text, int line);
void writeStats(EditorData ed);
void resetLine(WINDOW* w, int line, int dimX);
void writeTitle(char* titulo);
void changeLineColor(int line, int colorPair);
void writeToServer(int fdServ, int* run, char* user);
void readFromServer(int fdCli, int* run, EditorData* ed);

// Mutexes
pthread_mutex_t mutexEditor;

WINDOW* titleWin;
WINDOW* userWin;
WINDOW* lineWin;
WINDOW* editorWin;
WINDOW* statsWin;

int y = 0, x = 0;

/**
 * Verifica se ao inicializar o programa do cliente foi introduzido algum argumento.
 * @param argc numero de argumentos
 * @param argv argumentos
 * @param pipeName nome do pipe principal do servidor
 * @param user nome de utilizador
 * @return 0 caso tenha inserido utilizador, 1 caso contrário.
 */
int checkArgs(int argc, char** argv, char* pipeName, char* user) {
    int flag = 1;
    if (argc >= 3 && argc <= 5) {
        char *cmd;
        int res;

        while ((res = getopt(argc, argv, "u:p:")) != -1) {
            switch (res) {
                case 'u':
                    flag = 0;
                    cmd = optarg;
                    strncpy(user, cmd, 8);
                    break;
                case 'p':
                    cmd = optarg;
                    strncpy(pipeName, cmd, PIPE_NAME_MAX);
                    break;
            }
        }
    }
    return flag;
}

/**
 * Função que pede o nome de utilizador e pede para que o servidor verifque se existe, fazendo assim o login ou não.
 * @param user nome de utilizador
 */
void loginSession(char* user) {
    printf("Insira o nome de utilizador: ");
    scanf(" %8s", user);
}

/**
 * Função responsável por tudo acerca do editor.
 * @param user nome de utilizador
 * @param ed ponteiro para estrutura dos dados do editor
 * @param fdCli descritor do pipe do cliente
 * @param fdServ descritor do pipe do servidor
 * @param run variável de controlo de execução do cliente
 */
void editor(char* user, EditorData * ed, int fdCli, int fdServ, int* run) {
    puts("Entrei no editor");
    initscr();
    start_color();
    clear();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);

    titleWin = createSubWindow(stdscr, WIN_TITLE_MAX_Y, WIN_TITLE_MAX_X, 0, 0);
    userWin = createSubWindow(stdscr, ed->lin, WIN_USER_MAX_X, WIN_TITLE_MAX_Y + 1, 0);
    lineWin = createSubWindow(stdscr, ed->lin, WIN_LINENUM_MAX_X, WIN_TITLE_MAX_Y + 1, WIN_USER_MAX_X + 1);
    editorWin = createSubWindow(stdscr, ed->lin, ed->col, WIN_TITLE_MAX_Y + 1, WIN_LINENUM_MAX_X + WIN_USER_MAX_X + 2);
    statsWin = createSubWindow(stdscr, 3, WIN_TITLE_MAX_X, WIN_TITLE_MAX_Y + ed->lin + 2, 0);

    configureWindow(userWin, COLOR_PAIR(3));
    configureWindow(lineWin, COLOR_PAIR(3));
    configureWindow(editorWin, COLOR_PAIR(2));
    configureWindow(titleWin, COLOR_PAIR(1));
    configureWindow(statsWin, COLOR_PAIR(3));

    writeTitle(ed->fileName);
    writeUsers(*ed);
    writeLineNumbers(ed->lin);

    wmove(editorWin, 0, 0);
    wrefresh(stdscr);
    wrefresh(editorWin);

    werase(editorWin);
    writeDocument(ed->lines, ed->lin);
    wrefresh(editorWin);

    pthread_mutex_init(&mutexEditor, NULL);

    //Preparar FD para select
    fd_set fd_leitura, fd_leitura_temp;
    FD_ZERO(&fd_leitura);
    FD_ZERO(&fd_leitura_temp);
    FD_SET(STDIN_FILENO, &fd_leitura);
    FD_SET(fdCli, &fd_leitura);

    int key;
    refreshCursor(y, x, ed->lin);

    while (*run) {
        fd_leitura_temp = fd_leitura;
        switch (select(32, &fd_leitura_temp, NULL, NULL, NULL)) {
            case -1:
                endwin();
                puts("Erro no select");
                *run = 0;
                break;
            case 0:
                break;
            default:
                if (FD_ISSET(STDIN_FILENO, &fd_leitura_temp)) {
                    wattroff(lineWin, COLOR_PAIR(4));
                    writeLineNumbers(ed->lin);
                    refreshCursor(y, x, ed->lin);
                    writeToServer(fdServ, run, user);
                } else if (FD_ISSET(fdCli, &fd_leitura_temp)) {
                    readFromServer(fdCli, run, ed);
                }
        }
    }
    endwin();
    return;
}

/**
 * Função responsável por ler um character e enviar para o servidor.
 * @param fdServ descritor do pipe do servidor
 * @param run variável de controlo de execução do cliente
 * @param user nome de utilizador
 */
void writeToServer(int fdServ, int* run, char* user) {
    int key;
    ClientMsg msg;

    key = getch();

    pthread_mutex_lock(&mutexEditor);

    strncpy(msg.username, user, 9);

    switch (key) {
        case KEY_LEFT:
            msg.msgType = MOVE_LEFT;
            break;
        case KEY_RIGHT:
            msg.msgType = MOVE_RIGHT;
            break;
        case KEY_UP:
            msg.msgType = MOVE_UP;
            break;
        case KEY_DOWN:
            msg.msgType = MOVE_DOWN;
            break;
        case KEY_ENTR:
            msg.msgType = K_ENTER;
            break;
        case KEY_BACKSPACE:
            msg.msgType = K_BACKSPACE;
            break;
        case KEY_DELETE:
            msg.msgType = K_DEL;
            break;
        case KEY_ESC:
            msg.msgType = K_ESC;
            break;
        default:
            msg.msgType = K_CHAR;
            msg.letra = key;
    }
    if (*run) {
        write(fdServ, &msg, sizeof (msg));
    }
    pthread_mutex_unlock(&mutexEditor);
}

/**
 * Função responsável por receber mensagens do servidor e atualizar os dados respectivos.
 * @param fdCli descritor do pipe do cliente
 * @param run variável de controlo de execução do cliente
 * @param ed ponteiro para estrutura de dados do editor
 */
void readFromServer(int fdCli, int* run, EditorData *ed) {
    int nBytes;
    ServerMsg msg;
    int serverUp = 1;

    nBytes = read(fdCli, &msg, sizeof (msg));

    pthread_mutex_lock(&mutexEditor);

    if (nBytes == sizeof (msg)) {
        *ed = msg.ed;
        y = msg.cursorLinePosition;
        x = msg.cursorColumnPosition;

        switch (msg.code) {
            case SERVER_SHUTDOWN:
                serverUp = 0;
                break;
            case EDITOR_SHUTDOWN:
                refreshCursor(msg.cursorLinePosition, msg.cursorColumnPosition, ed->lin);
                *run = 0;
                break;
            case ASPELL_ERROR:
                changeLineColor(msg.cursorLinePosition, 4);
                refreshCursor(msg.cursorLinePosition, msg.cursorColumnPosition, ed->lin);
                break;
            case STATS_UPDATE:
                writeStats(*ed);
                refreshCursor(msg.cursorLinePosition, msg.cursorColumnPosition, ed->lin);
                break;
            case TIMEOUT:
                wattroff(lineWin, COLOR_PAIR(4));
                writeLineNumbers(ed->lin);
            default:
                writeUsers(*ed);
                writeTitle(ed->fileName);
                writeDocument(ed->lines, ed->lin);
                refreshCursor(msg.cursorLinePosition, msg.cursorColumnPosition, ed->lin);
                break;
        }
    }
    pthread_mutex_unlock(&mutexEditor);

    if (!serverUp)
        exitServerShutdown();
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
 * @param titulo nome do ficheiro
 */
void writeTitle(char* titulo) {
    werase(titleWin);
    mvwprintw(titleWin, 0, 0, "%s - MEDIT", titulo);
    wrefresh(titleWin);
}

/**
 * Função responsável por escrever o número de identificação na respetiva linha enviada como argumento.
 * @param i linha
 */
void writeOneLineNumber(int i) {
    mvwprintw(lineWin, i, 0, "%02d", i);
}

/**
 * Função responsável por escrever a identicação numerada de cada linha.
 * @param lines quantidade de linhas
 */
void writeLineNumbers(int lines) {
    for (int i = 0; i < lines; i++) {
        writeOneLineNumber(i);
    }
    wrefresh(lineWin);
}

/**
 * Função responsável por escrever o nome do utilizador que está a editar a linha.
 * @param ed estrutura de dados do editor
 */
void writeUsers(EditorData ed) {
    for (int i = 0; i < ed.lin; i++) {
        mvwprintw(userWin, i, 0, "%s", ed.clients[i]);
    }
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
 * Função responsável por escrever as estatísticas do editor na sua respectiva janela.
 * @param ed estrutura de dados do editor
 */
void writeStats(EditorData ed) {
    werase(statsWin);
    mvwprintw(statsWin, 0, 1, "Numero de palavras: %d", ed.numWords);
    mvwprintw(statsWin, 1, 1, "Numero de letras: %d", ed.numLetters);

    mvwprintw(statsWin, 2, 1, "Caracteres mais comuns: %c\t%c\t%c\t%c\t%c", ed.mostCommonChars[0],
            ed.mostCommonChars[1], ed.mostCommonChars[2], ed.mostCommonChars[3], ed.mostCommonChars[4]);

    wrefresh(statsWin);
}

/**
 * Função responsável por escrever um array de linhas no ecrã.
 * @param text string a ser escrita
 * @param line número da linha onde escrever
 */
void writeTextLine(char* text, int line) {
    mvwprintw(editorWin, line, 0, "%s", text);
    wrefresh(editorWin);
}

/**
 * Função responsável por atualizar o cursor e as suas respectivas coordenadas.
 * @param y linha
 * @param x coluna
 * @param lines quantidade de linhas
 */
void refreshCursor(int y, int x, int lines) {
    int cy, cx;
    wmove(editorWin, y, x);
    getyx(editorWin, cy, cx);
    //mvwprintw(stdscr, WIN_TITLE_MAX_Y + lines + 2, 0, "l: %02d\tc: %02d", cy, cx);
    wrefresh(stdscr);
    wrefresh(editorWin);
}

/**
 * Função responsável por alterar a cor de uma determinada linha.
 * @param line identificacao da linha
 * @param colorPair id do par de cores
 */
void changeLineColor(int line, int colorPair) {
    wattron(lineWin, COLOR_PAIR(colorPair));
    writeOneLineNumber(line);
    wrefresh(lineWin);
    //wattroff(editorWin, COLOR_PAIR(colorPair));
}