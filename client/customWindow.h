#ifndef __CUSTOMWINDOW_H__
#define __CUSTOMWINDOW_H__

// #include <ncurses.h>

WINDOW *create_newWin(int row, int col, int startR, int startC, int borderSize);
void destroy_win(WINDOW *local_win);
void eraseInline(WINDOW *local_win, int row, int colFrom, int colTo);
// window for user choose login/register
ScreenState menu_window(WINDOW *local_win, char *notify);
ScreenState inputLogin(WINDOW *local_win, char outUsername[], char outPassword[]);
ScreenState inputRegister(WINDOW *local_win, char outUsername[], char outPassword[]);

// window for user choose fight or rank
ScreenState port_window(WINDOW *local_win, char *notify);
ScreenState select_mode_window(WINDOW *local_win);

void rankUI(WINDOW *local_win, char *rankString);
void battleUI(WINDOW *local_win);
void betUI(WINDOW *local_win, char *username1, char *username2);
void ratioUI(WINDOW *local_win, char *username1, int rate1, char *username2, int rate2);
void messagesUI(WINDOW *local_win);
void messageContentUI(WINDOW *local_win);
void messageUI(WINDOW *local_win);
void totalViewersUI(WINDOW *local_win, int totalViewer);
void strengthSelectUI(WINDOW *local_win, int currentStrength);
void HPPlayerUI(WINDOW *HP_player1_win, WINDOW *HP_player2_win, int hp_player1, int hp_player2);
void allowAttackNotify(WINDOW *local_win, char *notify);

char *readFile(char *filename);
void characterWinUI(
    WINDOW *fight_win, char **character_1, char **character_2,
    int *count, int char1_length, int char2_length);

#endif