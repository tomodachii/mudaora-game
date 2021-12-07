#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>

#include "const.h"
#include "drawingFunc.h"

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

void delay(int seconds)
{
    clock_t start_time = clock();
    while (clock() < start_time + seconds * 100)
        ;
}

void gotoxy(int x, int y)
{
    printf("%c[%d;%df", 0x1B, y, x);
}

/*------------------------*/

// xoa 1 khu vuc theo hinh chu nhat
void clearRect(int start_x, int start_y, int width, int height)
{
    for (int j = start_y; j < start_y + height; j++)
    {
        gotoxy(start_x, j);
        for (int i = start_x; i <= start_x + width; i++)
        {

            printf(" ");
        }
    }
}

// xoa 1 khu vuc theo hinh chu nhat, nhung bat dau nguoc chieu Ox
void clearRectReverse(int start_x, int start_y, int width, int height)
{
    for (int i = start_x - width - 1; i <= start_x; i++)
    {
        for (int j = start_y - 1; j < start_y + height; j++)
        {
            gotoxy(i, j);
            printf(" ");
        }
    }
}

// xoa man hinh
void clearScreen()
{
    clearRect(0, 0, WIDTH, HEIGHT);
}

// ve vien
void drawBorder()
{
    clearScreen();
    printf(KYEL);
    for (int i = 1; i < WIDTH; i++)
    {
        gotoxy(i, 2);
        putchar('x');
    }
    for (int i = 1; i < WIDTH; i++)
    {
        gotoxy(i, HEIGHT);
        putchar('x');
    }
    for (int i = 3; i < HEIGHT; i++)
    {
        gotoxy(0, i);
        putchar('X');
    }
    for (int i = 3; i < HEIGHT; i++)
    {
        gotoxy(WIDTH - 1, i);
        putchar('X');
    }
}

// dinh dang xuat chuoi mean
// cach tren top, cach phai, cach trai
void printText(char *str, int top, int left, int right)
{
    int c = 0, line = 0;
    while (str[c] != '\0')
    {
        gotoxy(left, top + line++);
        for (int i = left; i < right; i++)
        {
            if (str[c] != '\0')
            {
                printf("%c", str[c++]);
            }
            else
            {
                break;
            }
        }
    }
}

void drawSelectMenu()
{
    system("clear");
    drawBorder();

    int X_POSITION = MARGIN_LEFT + 5 * WIDTH / 12,
        Y_POSITION = TOP + 5 * HEIGHT / 12;

    // danh sach cac lua chon
    printf(KWHT);
    gotoxy(X_POSITION, Y_POSITION);
    printf("Mode 1");
    gotoxy(X_POSITION, Y_POSITION + 2);
    printf("Mode 2");
    gotoxy(X_POSITION, Y_POSITION + 4);
    printf("Quit");

    int pointer = Y_POSITION;
    printf(KYEL);

    while (state == MAIN_MENU)
    {
        sleep(0.3);
        // phat hien nhan phim
        if (kbhit())
        {
            // xoa con tro o vi tri cu
            // gotoxy(X_POSITION - 3, pointer);
            // printf("  ");
            gotoxy(X_POSITION - 10, pointer);
            putchar(' ');
            gotoxy(X_POSITION + 15, pointer);
            putchar(' ');
            gotoxy(X_POSITION - 10, pointer - 1);
            for (int i = 0; i < 26; i++)
            {
                putchar(' ');
            }
            gotoxy(X_POSITION - 10, pointer + 1);
            for (int i = 0; i < 26; i++)
            {
                putchar(' ');
            }
            // lay ma ascii cu phim vua nhan
            char key = getch();
            // up
            if (key == 65)
            {
                if (pointer == Y_POSITION)
                {
                    // neu con tro dang o vi tri cao nhat thi chuyen xuong cuoi
                    pointer = Y_POSITION + 4;
                }
                else
                {
                    pointer -= 2;
                }
            }
            // down
            if (key == 66)
            {
                if (pointer == Y_POSITION + 4)
                {
                    // neu dang o duoi cung thi chuyen len tren
                    pointer = Y_POSITION;
                }
                else
                {
                    pointer += 2;
                }
            }
            // Enter
            if (key == 10)
            {
                state = (pointer - Y_POSITION) / 2 + 1;
            }
        }

        printf(KYEL);

        gotoxy(X_POSITION - 10, pointer);
        putchar('|');
        gotoxy(X_POSITION + 15, pointer);
        putchar('|');
        gotoxy(X_POSITION - 10, pointer - 1);
        for (int i = 0; i < 26; i++)
        {
            putchar('~');
        }
        gotoxy(X_POSITION - 10, pointer + 1);
        for (int i = 0; i < 26; i++)
        {
            putchar('~');
        }

        gotoxy(0, 0);
        printf(KWHT);
        printf("      ");
        gotoxy(0, 0);
    }
}

void returnMenu()
{
    gotoxy(20, 1);
    printf("Press Enter to return to the menu");
    // while(getchar() != '\n');
    // nhan phin bat ky de chung trinh chay tiep
    getchar();
    state = MAIN_MENU;
}

void quit()
{
    drawBorder();
    gotoxy(MARGIN_LEFT + 3 * WIDTH / 10 + 6, TOP + HEIGHT / 3);
    printf("__________SEE YOU AGAIN!__________");

    gotoxy(MARGIN_LEFT + 3 * WIDTH / 10 + 5, TOP + HEIGHT / 3 + 1);
    printf("|                                  |");

    gotoxy(MARGIN_LEFT + 3 * WIDTH / 10 + 1, TOP + HEIGHT / 3 + 2);
    printf("_____MADE BY \"POWER OF FRIENDSHIP\" TEAM_____");

    // dua con tro den duoi cua so de sao khi ket thuc phien lam viec
    // xuat hien dong lenh cua linux se ko lam hong cua so
    gotoxy(0, HEIGHT + 1);
}

void testDraw()
{
    system("clear");
    drawBorder();

    int X_POSITION = WIDTH / 2 - 16,
        Y_POSITION = TOP + 5 * HEIGHT / 12;

    printf(KWHT);
    gotoxy(X_POSITION, Y_POSITION);
    putchar('[');
    gotoxy(X_POSITION + 31, Y_POSITION);
    putchar(']');

    int currentChar = 'a';

    int checkPos = 1, checkVector = 0;

    while (1)
    {
        gotoxy(X_POSITION, Y_POSITION - 2);
        printf(KYEL);
        printf("Press up/down to start, press Enter to quit");
        gotoxy(0, 0);
        printf(KWHT);
        printf("        ");
        gotoxy(0, 0);

        sleep(0.3);
        // phat hien nhan phim
        if (kbhit())
        {
            // lay ma ascii cu phim vua nhan
            char key = getch();

            if (key == 65 || key == 66)
            {
                gotoxy(X_POSITION, Y_POSITION - 2);
                printf(KYEL);
                printf("Press Enter to stop                        ");
                gotoxy(0, 0);
                printf(KWHT);
                printf("        ");
                gotoxy(0, 0);

                while (1)
                {
                    delay(100);
                    if (checkVector == 0) // dang tang
                    {
                        gotoxy(X_POSITION + checkPos, Y_POSITION);
                        printf(KRED);
                        putchar('/');
                        if (checkPos == 30)
                        {
                            checkVector = -1;
                        }
                        else
                        {
                            checkPos++;
                        }
                    }
                    else // dang giam
                    {
                        gotoxy(X_POSITION + checkPos, Y_POSITION);
                        putchar(' ');
                        if (checkPos == 1)
                        {
                            checkVector = 0;
                        }
                        else
                        {
                            checkPos--;
                        }
                    }

                    gotoxy(X_POSITION, Y_POSITION + 2);
                    printf(KYEL);
                    printf("Current char: ");
                    putchar(currentChar);
                    if (currentChar == 'z') // chay tu a->z
                    {
                        currentChar = 'a';
                    }
                    else
                    {
                        currentChar++;
                    }
                    gotoxy(0, 0);
                    printf(KWHT);
                    printf("        ");
                    gotoxy(0, 0);

                    if (kbhit())
                    {
                        char key2 = getch();
                        if (key2 == 10)
                        {
                            break;
                        }
                    }
                }
            }
            // Enter
            else if (key == 10)
            {
                state = MAIN_MENU;
                break;
            }
        }

        // gotoxy(0, 0);
        // printf(KWHT);
        // printf("        ");
        // gotoxy(0, 0);
    }
}

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
    return str;
}

void drawCharacter(char *str, int x_position, int y_position)
{
    int lineNum = 0, colNum = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == '\n')
        {
            gotoxy(x_position, y_position + ++lineNum);
            colNum = 0;
        }
        else
        {
            if (str[i] == ' ')
            {
                gotoxy(x_position + colNum + 1, y_position + lineNum);
            }
            else
            {
                printf("%c", str[i]);
            }
            colNum++;
        }
    }
}

void drawCharacterReverse(char *str, int x_position, int y_position)
{
    int lineNum = 0, colNum = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == '\n')
        {
            gotoxy(x_position, y_position + ++lineNum);
            colNum = 0;
        }
        else
        {
            if (str[i] != ' ')
            {
                gotoxy(x_position - colNum, y_position + lineNum);
                printf("%c", str[i]);
            }
            colNum++;
        }
    }
}

void drawStand()
{
    clearScreen();
    drawBorder();

    char **str_stand = (char **)malloc(4 * sizeof(char *));
    str_stand[0] = readFile("./../text/stand_00.txt");
    str_stand[1] = readFile("./../text/stand_01.txt");
    str_stand[2] = readFile("./../text/stand_02.txt");
    str_stand[3] = readFile("./../text/stand_03.txt");
    int checkTurnStand = 0, checkVector = 0;

    char **str_ready = (char **)malloc(4 * sizeof(char *));
    str_ready[0] = readFile("./../text/ready_00.txt");
    str_ready[1] = readFile("./../text/ready_01.txt");
    str_ready[2] = readFile("./../text/ready_02.txt");
    str_ready[3] = readFile("./../text/ready_03.txt");
    int checkTurnReady = 0;

    char **vclText = (char **)malloc(5 * sizeof(char *));
    vclText[0] = readFile("./../text/text_00.txt");
    vclText[1] = readFile("./../text/text_01.txt");
    vclText[2] = readFile("./../text/text_02.txt");
    vclText[3] = readFile("./../text/text_03.txt");
    vclText[4] = readFile("./../text/text_04.txt");
    // int check = 0;

    while (1)
    {
        delay(3000);

        printf(KGRN);
        clearRect(5, 12, 32, 28);
        drawCharacter(str_stand[checkTurnStand], 5, 12);

        if (checkVector == 0)
        {
            if (checkTurnStand == 3)
                checkVector = -1;
            else
                checkTurnStand++;
        }
        else
        {
            if (checkTurnStand == 0)
                checkVector = 0;
            else
                checkTurnStand--;
        }

        printf(KCYN);
        clearRectReverse(130, 12, 32, 28);
        drawCharacterReverse(str_ready[checkTurnReady], 130, 12);

        if (checkTurnReady == 3)
        {
            checkTurnReady = 0;
        }
        else
        {
            checkTurnReady++;
        }

        // clearRect(45, 10, 55, 10);
        // printf(KRED);
        // drawCharacter(vclText[check], 45, 10);
        // if (check == 4)
        // {
        //     check = 0;
        // }
        // else
        // {
        //     check++;
        // }

        gotoxy(0, 0);
        printf(KWHT);
        printf("        ");
        gotoxy(0, 0);

        // phat hien nhan phim
        if (kbhit())
        {
            // lay ma ascii cu phim vua nhan
            char key = getch();
            if (key == 10)
            {
                state = MAIN_MENU;
                break;
            }
        }
    }
}

void drawMainMenu()
{
    system("clear");
    drawBorder();

    int X_POSITION = MARGIN_LEFT + 5 * WIDTH / 12,
        Y_POSITION = TOP + 5 * HEIGHT / 12;

    // danh sach cac lua chon
    printf(KWHT);
    gotoxy(X_POSITION, Y_POSITION);
    printf("LOGIN");
    gotoxy(X_POSITION, Y_POSITION + 2);
    printf("SIGN UP");
    gotoxy(X_POSITION, Y_POSITION + 4);
    printf("QUIT");

    int pointer = Y_POSITION;
    printf(KYEL);

    while (state == MAIN_MENU)
    {
        sleep(0.3);
        // phat hien nhan phim
        if (kbhit())
        {
            // xoa con tro o vi tri cu

            gotoxy(X_POSITION - 10, pointer);
            putchar(' ');
            gotoxy(X_POSITION + 15, pointer);
            putchar(' ');
            gotoxy(X_POSITION - 10, pointer - 1);
            for (int i = 0; i < 26; i++)
            {
                putchar(' ');
            }
            gotoxy(X_POSITION - 10, pointer + 1);
            for (int i = 0; i < 26; i++)
            {
                putchar(' ');
            }
            // lay ma ascii cu phim vua nhan
            char key = getch();
            // up
            if (key == 65)
            {
                if (pointer == Y_POSITION)
                {
                    // neu con tro dang o vi tri cao nhat thi chuyen xuong cuoi
                    pointer = Y_POSITION + 4;
                }
                else
                {
                    pointer -= 2;
                }
            }
            // down
            if (key == 66)
            {
                if (pointer == Y_POSITION + 4)
                {
                    // neu dang o duoi cung thi chuyen len tren
                    pointer = Y_POSITION;
                }
                else
                {
                    pointer += 2;
                }
            }
            // Enter
            if (key == 10)
            {
                state = (pointer - Y_POSITION) / 2 + 1;
            }
        }

        printf(KYEL);

        gotoxy(X_POSITION - 10, pointer);
        putchar('|');
        gotoxy(X_POSITION + 15, pointer);
        putchar('|');
        gotoxy(X_POSITION - 10, pointer - 1);
        for (int i = 0; i < 26; i++)
        {
            putchar('~');
        }
        gotoxy(X_POSITION - 10, pointer + 1);
        for (int i = 0; i < 26; i++)
        {
            putchar('~');
        }

        gotoxy(0, 0);
        printf(KWHT);
        printf("      ");
        gotoxy(0, 0);
    }
}

void getInput(char key, char *str, int *index, int X_POSITION, int Y_POSITION)
{
    if (key == 127)
    {
        // del 1 ky tu
        if ((*index) > 0)
        {
            str[--(*index)] = '\0';
        }
    }

    if ((key >= 'a' && key <= 'z'))
    {
        str[(*index)++] = key;
        str[(*index)] = '\0';
    }

    printf(KWHT);
    gotoxy(X_POSITION + 10, Y_POSITION);
    printf("%s", str);
}

// Ve con tro Login Page
void drawPointerLoginPage(int choice){ 
    if(choice == 1 || choice == 2){
        go
    } else{

    }
}

void drawLoginPage()
{
    clearScreen();
    drawBorder();

    int X_POSITION = MARGIN_LEFT + 4 * WIDTH / 12,
        Y_POSITION = TOP + 4 * HEIGHT / 12;

    // danh sach cac lua chon
    printf(KYEL);
    gotoxy(X_POSITION, Y_POSITION);
    printf("Username: ");
    gotoxy(X_POSITION, Y_POSITION + 2);
    printf("Password: ");
    gotoxy(X_POSITION, Y_POSITION + 5);
    printf("Login");
    gotoxy(X_POSITION + 10, Y_POSITION + 5);
    printf("Return");

    int pointer = Y_POSITION;
    int choice = 1;
    printf(KYEL);

    char username[20], password[20];
    int indexUsername = 0, indexPassword = 0;

    while (1)
    {
        // phat hien nhan phim
        if (kbhit())
        {
            // xoa con tro o vi tri cu

            gotoxy(X_POSITION - 4, pointer);
            printf("  ");

            // lay ma ascii cu phim vua nhan
            char key = getch();
            // up
            if (key == 65 || key == 66)
            {
                if (key == 65)
                {
                    if (choice == 1)
                    {
                        // neu con tro dang o vi tri cao nhat thi chuyen xuong cuoi
                        choice = 4;
                    }
                    else
                    {
                        choice--;
                    }
                }
                // down
                else if (key == 66)
                {
                    if (choice == 4)
                    {
                        // neu dang o duoi cung thi chuyen len tren
                        choice = 1;
                    }
                    else
                    {
                        choice++;
                    }
                }
            }
            // Enter
            else if (key == 10)
            {
                state = MAIN_MENU;
                break;
            }
            // Nhap username/password tu ban phim
            else
            {
                if (pointer == Y_POSITION)
                {
                    getInput(key, username, &indexUsername, X_POSITION, Y_POSITION);
                }
                else if (pointer == Y_POSITION + 2)
                {
                    getInput(key, password, &indexPassword, X_POSITION, Y_POSITION + 2);
                }
            }
        }

        printf(KYEL);

        gotoxy(X_POSITION - 4, pointer);
        printf("<>");

        printf(KWHT);
        if (pointer == Y_POSITION)
        {
            gotoxy(X_POSITION + 10 + indexUsername, Y_POSITION);
            printf("      ");
            gotoxy(X_POSITION + 10 + indexUsername, Y_POSITION);
        }
        else if (pointer == Y_POSITION + 2)
        {
            gotoxy(X_POSITION + 10 + indexPassword, Y_POSITION + 2);
            printf("      ");
            gotoxy(X_POSITION + 10 + indexPassword, Y_POSITION + 2);
        }
        else
        {
            gotoxy(0, 0);
            printf("      ");
            gotoxy(0, 0);
        }
    }
}