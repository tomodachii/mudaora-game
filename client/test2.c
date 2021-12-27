#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

char *readFile(char *filename)
{
    FILE *fp;
    fp = fopen(filename, "r+");

    if (fp == NULL)
    {
        printf("\nCan't open file!\n");
        exit(1);
    }

    char *str = malloc(10000 * sizeof(char));
    int index = 0;
    while (1)
    {
        char c = getc(fp);
        if (c == EOF)
        {
            str[index] = '\0';
            break;
        }
        str[index++] = c;
    }

    str = realloc(str, index);
    fclose(fp);
    return str;
}

void copyString(char *str1, char *str2)
{
    for (int i = 0; i < strlen(str2); i++)
    {
        str1[i] = str2[i];
    }
    str1[strlen(str1)] = '\0';
    printf(":)) %s\n", str1);
}

void readF()
{
    FILE *f1 = fopen("./../text/stand_03.txt", "r");
    FILE *f2 = fopen("./../text/reverse/stand_03_reverse.txt", "w");

    char str[40][100];
    int index[40], line = 0, check = 0;
    memset(index, 0, 40);
    while (1)
    {

        char c = fgetc(f1);
        if (c == EOF)
        {
            str[line][index[line]] = '\0';
            line++;
            break;
        }
        if (c == '\n')
        {
            line++;
            index[line] = 0;
        }
        else
            str[line][index[line]++] = c;
    }
    for (int i = 0; i < line; i++)
    {
        // printf("%d\n", check++);
        if (strlen(str[i]) != 0)
        {
            for (int j = index[i]; j < strlen(str[line - 1]); j++)
            {
                str[i][j] = ' ';
            }
            index[i] = strlen(str[line - 1]);
            str[i][strlen(str[line - 1])] = '\0';
        }
    }
    // printf("%d\n", check++);
    for (int i = 0; i < line; i++)
    {
        // printf("%d\n", check++);
        for (int j = index[i] - 1; j >= 0; j--)
        {
            fprintf(f2, "%c", str[i][j]);
        }
        fprintf(f2, "\n");
    }
}

// int main()
// {
//     // readF();

//     char **str_stand = (char **)malloc(6 * sizeof(char *));
//     str_stand[0] = readFile("./../text/stand_00.txt");
//     str_stand[1] = readFile("./../text/stand_01.txt");
//     str_stand[2] = readFile("./../text/stand_02.txt");
//     str_stand[3] = readFile("./../text/stand_03.txt");
//     str_stand[4] = readFile("./../text/stand_02.txt");
//     str_stand[5] = readFile("./../text/stand_01.txt");

//     char **str_stand_reverse = (char **)malloc(4 * sizeof(char *));
//     str_stand_reverse[0] = readFile("./../text/reverse/ready_00_reverse.txt");
//     str_stand_reverse[1] = readFile("./../text/reverse/ready_01_reverse.txt");
//     str_stand_reverse[2] = readFile("./../text/reverse/ready_02_reverse.txt");
//     str_stand_reverse[3] = readFile("./../text/reverse/ready_03_reverse.txt");

//     char **str_stand_reverse = (char **)malloc(6 * sizeof(char *));
//     str_stand_reverse[0] = readFile("./../text/reverse/stand_00_reverse.txt");
//     str_stand_reverse[1] = readFile("./../text/reverse/stand_01_reverse.txt");
//     str_stand_reverse[2] = readFile("./../text/reverse/stand_02_reverse.txt");
//     str_stand_reverse[3] = readFile("./../text/reverse/stand_03_reverse.txt");
//     str_stand_reverse[4] = readFile("./../text/reverse/stand_02_reverse.txt");
//     str_stand_reverse[5] = readFile("./../text/reverse/stand_01_reverse.txt");

//     char **str_ready_reverse = (char **)malloc(6 * sizeof(char *));
//     str_ready_reverse[0] = readFile("./../text/reverse/ready_00_reverse.txt");
//     str_ready_reverse[1] = readFile("./../text/reverse/ready_01_reverse.txt");
//     str_ready_reverse[2] = readFile("./../text/reverse/ready_02_reverse.txt");
//     str_ready_reverse[3] = readFile("./../text/reverse/ready_03_reverse.txt");

//     int currStr = 0;

//     initscr();
//     raw();
//     noecho();

//     start_color();

//     init_pair(1, COLOR_YELLOW, COLOR_BLACK);
//     init_pair(2, COLOR_GREEN, COLOR_BLACK);
//     init_pair(3, COLOR_RED, COLOR_BLACK);
//     attron(COLOR_PAIR(1));

//     box(stdscr, 0, 0);
//     attroff(COLOR_PAIR(1));
//     refresh();

//     keypad(stdscr, TRUE);

//     int y, x, col, row;
//     y = getmaxy(stdscr);
//     WINDOW *test_win = newwin(40, 40, y - 41, 2);
//     WINDOW *test_win_reverse = newwin(40, 60, y - 41, 41);
//     wattron(test_win, COLOR_PAIR(2) | A_BOLD);
//     wattron(test_win_reverse, COLOR_PAIR(3)  | A_BOLD);
//     while (currStr != 24)
//     {
//         werase(test_win);
//         werase(test_win_reverse);
//         box(test_win, 0, 0);
//         box(test_win_reverse, 0, 0);
//         mvwprintw(test_win, y - 32, 0, "%s", str_stand[currStr % 6]);
//         mvwprintw(test_win_reverse, y - 32, 0, "%s", str_stand_reverse[currStr % 4]);
//         // printf("%s", str_stand[0]);
//         curs_set(0);
//         wrefresh(test_win);
//         wrefresh(test_win_reverse);
//         currStr++;
//         napms(300);
//     }
//     wattroff(test_win, COLOR_PAIR(2) | A_BOLD);
//     wattroff(test_win_reverse, COLOR_PAIR(3)  | A_BOLD);

//     getch();
//     delwin(test_win);
//     delwin(test_win_reverse);
//     endwin();

//     // char str[50] = "asdsdsa \x1B[31m asdasdasdaszxcz";
//     // char vcl[80];
//     // strcpy(vcl, str_stand[0]);
//     // strcat(vcl, str);
//     // printf("%s\n", vcl);
//     // copyString(vcl, str_stand[0]);
//     return 0;
// }

void draw_character(WINDOW *local_win, int y, int x, char *str, int color_pair)
{
    int len = strlen(str), line = 0, index_char = 0;

    wattron(local_win, COLOR_PAIR(color_pair));
    wmove(local_win, y, x);
    for (int i = 0; i < len; i++)
    {
        if (str[i] == '\n')
        {
            wmove(local_win, y + ++line, x);
            index_char = 0;
        }
        else
        {
            if (str[i] == ' ')
            {
                wmove(local_win, y + line, x + ++index_char);
            }
            else
            {
                waddch(local_win, str[i]);
                index_char++;
            }
        }
    }
    wattroff(local_win, COLOR_PAIR(color_pair));
}

void draw_character_reverse(WINDOW *local_win, int y, int x, char *str, int color_pair)
{
    int len = strlen(str), line = 0, index_char = 0;

    wattron(local_win, COLOR_PAIR(color_pair));
    wmove(local_win, y, x);
    for (int i = 0; i < len; i++)
    {
        if (str[i] == '\n')
        {
            wmove(local_win, y + ++line, x);
            index_char = 0;
        }
        else
        {
            wmove(local_win, y + line, x - ++index_char);
            if (str[i] == ' ')
            {
            }
            else
            {
                waddch(local_win, str[i]);
                // index_char++;
            }
        }
    }
    wattroff(local_win, COLOR_PAIR(color_pair));
}

void draw_fight(
    WINDOW *fight_win, char **character_1, char **character_2,
    int y1, int x1, int y2, int x2, int *count, int char1_length, int char2_length)
{
    werase(fight_win);
    wattron(fight_win, COLOR_PAIR(2));
    box(fight_win, 0, 0);
    wattroff(fight_win, COLOR_PAIR(2));

    draw_character(fight_win, y1, x1, character_1[(*count) % char1_length], 3);
    draw_character_reverse(fight_win, y2, x2, character_2[(*count) % char2_length], 1);
    (*count)++;

    wrefresh(fight_win);
    napms(300);
}

int main()
{
    char **str_stand = (char **)malloc(6 * sizeof(char *));
    str_stand[0] = readFile("./../text/stand_00.txt");
    str_stand[1] = readFile("./../text/stand_01.txt");
    str_stand[2] = readFile("./../text/stand_02.txt");
    str_stand[3] = readFile("./../text/stand_03.txt");
    str_stand[4] = readFile("./../text/stand_02.txt");
    str_stand[5] = readFile("./../text/stand_01.txt");

    char **str_ready = (char **)malloc(6 * sizeof(char *));
    str_ready[0] = readFile("./../text/ready_00.txt");
    str_ready[1] = readFile("./../text/ready_01.txt");
    str_ready[2] = readFile("./../text/ready_02.txt");
    str_ready[3] = readFile("./../text/ready_03.txt");

    int str_len = strlen(str_stand[0]);

    char **str_punch = (char **)malloc(6 * sizeof(char *));
    str_punch[0] = (char *)malloc((str_len + 1) * sizeof(char));
    strcpy(str_punch[0], str_stand[0]);
    str_punch[1] = readFile("./../text/punch_01.txt");
    str_punch[2] = readFile("./../text/punch_02.txt");
    str_punch[3] = readFile("./../text/punch_03.txt");

    char **str_stun = (char **)malloc(6 * sizeof(char *));
    str_stun[0] = (char *)malloc((str_len + 1) * sizeof(char));
    str_stun[1] = (char *)malloc((str_len + 1) * sizeof(char));
    strcpy(str_stun[0], str_stand[0]);
    strcpy(str_stun[1], str_stand[0]);
    str_stun[2] = readFile("./../text/stun_02.txt");
    str_stun[3] = readFile("./../text/stun_03.txt");

    int currStr = 0;

    initscr();
    raw();
    noecho();

    start_color();

    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(1));
    refresh();

    keypad(stdscr, TRUE);

    int y, x, col, row;
    getmaxyx(stdscr, y, x);

    /***********************************************************/

    int width_fight_win = x * 7 / 10;

    WINDOW *cmt_list_win = derwin(stdscr, y - 12, x - width_fight_win - 2, 1, width_fight_win + 1);
    wattron(cmt_list_win, COLOR_PAIR(3));
    box(cmt_list_win, 0, 0);
    wattroff(cmt_list_win, COLOR_PAIR(3));

    // wmove(cmt_list_win, 2, 2);
    // printf("[\u2588]");
    // mvwprintw(cmt_list_win, 2, 2, "[███████]");

    wrefresh(cmt_list_win);

    WINDOW *cmt_choice_win = derwin(stdscr, 10, x - width_fight_win - 2, y - 11, width_fight_win + 1);
    wattron(cmt_choice_win, COLOR_PAIR(4));
    box(cmt_choice_win, 0, 0);
    wattroff(cmt_choice_win, COLOR_PAIR(4));
    wrefresh(cmt_choice_win);

    WINDOW *fight_win = derwin(stdscr, y - 2, width_fight_win, 1, 1);
    wattron(fight_win, COLOR_PAIR(2));
    box(fight_win, 0, 0);
    wattroff(fight_win, COLOR_PAIR(2));
    int count = 0;
    while (count < 10)
    {
        draw_fight(fight_win, str_stand, str_stand, y - 31, 2, y - 31, 55, &count, 6, 4);
    }
    count = 0;
    while (count < 41)
    {
        draw_fight(fight_win, str_stun, str_punch, y - 31, 2, y - 31, 55, &count, 4, 4);
    }

    getch();

    /***********************************************************/

    delwin(fight_win);
    delwin(cmt_list_win);
    delwin(cmt_choice_win);
    endwin();

    return 0;
}