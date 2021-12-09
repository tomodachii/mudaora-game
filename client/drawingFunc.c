#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

#include "const.h"
#include "drawingFunc.h"
#include "logic.h"
#include "handle.h"

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
    gotoxy(1, 2);
    for (int i = 1; i < WIDTH; i++)
    {
        putchar('x');
    }
    gotoxy(1, HEIGHT);
    for (int i = 1; i < WIDTH; i++)
    {
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

// Ve khung phu ben trong khung chinh
void drawSubBorder(int x, int y, int width, int height, char *title)
{
    gotoxy(x, y);
    for (int i = 0; i < width; i++)
    {
        putchar('_');
    }
    gotoxy(x, y + height - 1);
    for (int i = 0; i < width; i++)
    {
        putchar('_');
    }

    for (int i = 1; i < height; i++)
    {
        gotoxy(x, i + y);
        putchar(220);
    }
    for (int i = 1; i < height; i++)
    {
        gotoxy(x + width, i + y);
        putchar(220);
    }

    gotoxy(x + width / 2 - strlen(title) / 2 - 1, y);
    printf(" %s ", title);
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
            else if (key == 66)
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
            else if (key == 10)
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
            else if (key == 66)
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
            else if (key == 10)
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

/**************LOGIN PAGE*************************/

void getInput(char key, char *str, int maxlen, int *index, int X_POSITION, int Y_POSITION)
{
    if (key == 127)
    {
        // del 1 ky tu
        if ((*index) > 0)
        {
            str[--(*index)] = '\0';
        }
    }
    if ((*index) < maxlen)
    {

        if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9'))
        {
            str[(*index)++] = key;
            str[(*index)] = '\0';

            printf(KWHT);
            gotoxy(X_POSITION + 10, Y_POSITION);
            printf("%s", str);
        }
    }
}

// Ve pointer cho page login
void drawPointerLoginPage(char key, int *choice, int X_POSITION, int Y_POSITION)
{
    if ((*choice) == 0 || (*choice) == 1)
    {
        gotoxy(X_POSITION - 4, Y_POSITION + 2 * (*choice));
        printf("  ");
    }
    else
    {
        if ((*choice) == 2)
        {
            gotoxy(X_POSITION - 2, Y_POSITION + 5);
            putchar(' ');
            gotoxy(X_POSITION + 6, Y_POSITION + 5);
            putchar(' ');
            gotoxy(X_POSITION - 2, Y_POSITION + 4);
            for (int i = 0; i < 9; i++)
            {
                putchar(' ');
            }
            gotoxy(X_POSITION - 2, Y_POSITION + 6);
            for (int i = 0; i < 9; i++)
            {
                putchar(' ');
            }
        }
        else
        {
            gotoxy(X_POSITION + 8, Y_POSITION + 5);
            putchar(' ');
            gotoxy(X_POSITION + 17, Y_POSITION + 5);
            putchar(' ');
            gotoxy(X_POSITION + 8, Y_POSITION + 4);
            for (int i = 0; i < 10; i++)
            {
                putchar(' ');
            }
            gotoxy(X_POSITION + 8, Y_POSITION + 6);
            for (int i = 0; i < 10; i++)
            {
                putchar(' ');
            }
        }
    }
    if (key == 65 || key == 68)
    {
        if ((*choice) == 0)
        {
            // neu con tro dang o vi tri cao nhat thi chuyen xuong cuoi
            (*choice) = 3;
        }
        else
        {
            (*choice)--;
        }
    }
    // down
    else if (key == 66 || key == 67 || key == 10)
    {
        if ((*choice) == 3)
        {
            // neu dang o duoi cung thi chuyen len tren
            (*choice) = 0;
        }
        else
        {
            (*choice)++;
        }
    }
    printf(KYEL);
    if ((*choice) == 0 || (*choice) == 1)
    {
        gotoxy(X_POSITION - 4, Y_POSITION + 2 * (*choice));
        printf("\u27A4");
    }
    else
    {
        if ((*choice) == 2)
        {
            gotoxy(X_POSITION - 2, Y_POSITION + 5);
            putchar('|');
            gotoxy(X_POSITION + 6, Y_POSITION + 5);
            putchar('|');
            gotoxy(X_POSITION - 2, Y_POSITION + 4);
            for (int i = 0; i < 9; i++)
            {
                putchar('=');
            }
            gotoxy(X_POSITION - 2, Y_POSITION + 6);
            for (int i = 0; i < 9; i++)
            {
                putchar('=');
            }
        }
        else
        {
            gotoxy(X_POSITION + 8, Y_POSITION + 5);
            putchar('|');
            gotoxy(X_POSITION + 17, Y_POSITION + 5);
            putchar('|');
            gotoxy(X_POSITION + 8, Y_POSITION + 4);
            for (int i = 0; i < 10; i++)
            {
                putchar('=');
            }
            gotoxy(X_POSITION + 8, Y_POSITION + 6);
            for (int i = 0; i < 10; i++)
            {
                putchar('=');
            }
        }
    }

    gotoxy(0, Y_POSITION + 3);
    printf("X        ");
}

// Ve trang Login
void drawLoginPage(int sockfd)
{
    clearScreen();
    drawBorder();

    int X_POSITION = MARGIN_LEFT + 5 * WIDTH / 12,
        Y_POSITION = TOP + 4 * HEIGHT / 12;

    // danh sach cac lua chon
    printf(KYEL);
    gotoxy(X_POSITION, Y_POSITION);
    printf("Username: ");
    gotoxy(X_POSITION, Y_POSITION + 2);
    printf("Password: ");
    printf(KMAG);
    gotoxy(X_POSITION, Y_POSITION + 5);
    printf("Login");
    gotoxy(X_POSITION + 10, Y_POSITION + 5);
    printf("Return");
    gotoxy(X_POSITION, Y_POSITION - 2);
    printf(KCYN);
    printf("LOGIN");
    gotoxy(X_POSITION - 12, Y_POSITION + 8);
    printf(KGRN);
    printf("--Press up/down to switch your choice--");

    int choice = 0;

    printf(KYEL);
    gotoxy(X_POSITION - 4, Y_POSITION);
    printf("\u27A4");

    char username[20], password[20], mesg[50];
    int usernameLen = 0, passwordLen = 0;

    while (1)
    {
        // phat hien nhan phim
        if (kbhit())
        {
            // xoa con tro o vi tri cu

            // lay ma ascii cu phim vua nhan
            char key = getch();
            // up
            if (key == 65 || key == 66 || key == 67 || key == 68)
            {
                gotoxy(X_POSITION - 12, Y_POSITION + 10);
                printf("                                                    ");
                drawPointerLoginPage(key, &choice, X_POSITION, Y_POSITION);
            }
            // Enter
            else if (key == 10)
            {
                if (choice == 0 || choice == 1)
                {
                    drawPointerLoginPage(key, &choice, X_POSITION, Y_POSITION);
                }
                else if (choice == 2) // Click vao LOGIN
                {
                    // gotoxy(0, 0);
                    // printf("username: %s - password: %s", username, password);

                    gotoxy(X_POSITION - 12, Y_POSITION + 10);
                    printf(KRED);
                    if (usernameLen == 0 || passwordLen == 0)
                    {
                        if (usernameLen == 0)
                        {
                            printf("--Please enter username--");
                        }
                        else if (passwordLen == 0)
                        {
                            printf("--Please enter password--");
                        }
                    }
                    else
                    {
                        strcpy(mesg, username);
                        strcat(mesg, "|");
                        strcat(mesg, password);
                        addToken(mesg, LOGIN_SIGNAL);
                        // gotoxy(10, 10);
                        // printf("%s", mesg);

                        if (send(sockfd, (void *)mesg, strlen(mesg), 0) < 0)
                        {
                            gotoxy(0, 0);
                            printf("error");
                        };
                        strcpy(mesg, "");

                        recv(sockfd, mesg, 50, 0);
                        mesg[4] = '\0';
                        // gotoxy(10, 0);
                        // printf("%s - %ld", mesg, strlen(mesg));

                        int tokenTotal;
                        char **data = words(mesg, &tokenTotal, "|");
                        SignalState signalState = (data[tokenTotal - 1][0] - '0') * 10 + data[tokenTotal - 1][1] - '0';

                        int result = data[0][0] - '0';

                        if (signalState == SUCCESS_SIGNAL)
                        {
                            printf(KGRN);
                            gotoxy(X_POSITION - 12, Y_POSITION + 10);
                            printf("--Login success! Press Enter to continue--");
                            while (1)
                            {
                                if (kbhit())
                                {
                                    char key3 = getch();
                                    if (key3 == 10)
                                    {
                                        drawReadyPage(sockfd);
                                        return;
                                    }
                                }
                            }
                        }
                        else
                        {
                            printf(KRED);
                            gotoxy(X_POSITION - 12, Y_POSITION + 10);
                            if (result == 1)
                            {
                                printf("--Username not exist--");
                            }
                            else if (result == 2)
                            {
                                printf("--Someone logged in this account--");
                            }
                            else
                            {
                                printf("--Wrong password--");
                            }
                        }
                    }

                    // gotoxy(0, 0);
                    // printf("%s, %s", username, password);
                }
                else if (choice == 3) // Click vao Return
                {
                    state = MAIN_MENU;
                    break;
                }
            }
            // Nhap username/password tu ban phim
            else
            {
                if (choice == 0)
                {
                    getInput(key, username, 8, &usernameLen, X_POSITION, Y_POSITION);
                }
                else if (choice == 1)
                {
                    getInput(key, password, 8, &passwordLen, X_POSITION, Y_POSITION + 2);
                }
            }
        }

        printf(KWHT);
        if (choice == 0)
        {
            gotoxy(X_POSITION + 10 + usernameLen, Y_POSITION);
            printf("      ");
            gotoxy(X_POSITION + 10 + usernameLen, Y_POSITION);
        }
        else if (choice == 1)
        {
            gotoxy(X_POSITION + 10 + passwordLen, Y_POSITION + 2);
            printf("      ");
            gotoxy(X_POSITION + 10 + passwordLen, Y_POSITION + 2);
        }
        else
        {
            gotoxy(0, 0);
            printf("      ");
            gotoxy(0, 0);
        }
    }
}

/**************SIGN UP PAGE*************************/

// Ve pointer cho page sign up
void drawPointerSignUpPage(char key, int *choice, int X_POSITION, int Y_POSITION)
{
    if ((*choice) == 0 || (*choice) == 1)
    {
        gotoxy(X_POSITION - 4, Y_POSITION + 2 * (*choice));
        printf("  ");
    }
    else
    {
        if ((*choice) == 2)
        {
            gotoxy(X_POSITION - 2, Y_POSITION + 5);
            putchar(' ');
            gotoxy(X_POSITION + 8, Y_POSITION + 5);
            putchar(' ');
            gotoxy(X_POSITION - 2, Y_POSITION + 4);
            for (int i = 0; i < 11; i++)
            {
                putchar(' ');
            }
            gotoxy(X_POSITION - 2, Y_POSITION + 6);
            for (int i = 0; i < 11; i++)
            {
                putchar(' ');
            }
        }
        else
        {
            gotoxy(X_POSITION + 10, Y_POSITION + 5);
            putchar(' ');
            gotoxy(X_POSITION + 19, Y_POSITION + 5);
            putchar(' ');
            gotoxy(X_POSITION + 10, Y_POSITION + 4);
            for (int i = 0; i < 10; i++)
            {
                putchar(' ');
            }
            gotoxy(X_POSITION + 10, Y_POSITION + 6);
            for (int i = 0; i < 10; i++)
            {
                putchar(' ');
            }
        }
    }
    if (key == 65 || key == 68)
    {
        if ((*choice) == 0)
        {
            // neu con tro dang o vi tri cao nhat thi chuyen xuong cuoi
            (*choice) = 3;
        }
        else
        {
            (*choice)--;
        }
    }
    // down
    else if (key == 66 || key == 67 || key == 10)
    {
        if ((*choice) == 3)
        {
            // neu dang o duoi cung thi chuyen len tren
            (*choice) = 0;
        }
        else
        {
            (*choice)++;
        }
    }
    printf(KYEL);
    if ((*choice) == 0 || (*choice) == 1)
    {
        gotoxy(X_POSITION - 4, Y_POSITION + 2 * (*choice));
        printf("\u27A4");
    }
    else
    {
        if ((*choice) == 2)
        {
            gotoxy(X_POSITION - 2, Y_POSITION + 5);
            putchar('|');
            gotoxy(X_POSITION + 8, Y_POSITION + 5);
            putchar('|');
            gotoxy(X_POSITION - 2, Y_POSITION + 4);
            for (int i = 0; i < 11; i++)
            {
                putchar('=');
            }
            gotoxy(X_POSITION - 2, Y_POSITION + 6);
            for (int i = 0; i < 11; i++)
            {
                putchar('=');
            }
        }
        else
        {
            gotoxy(X_POSITION + 10, Y_POSITION + 5);
            putchar('|');
            gotoxy(X_POSITION + 19, Y_POSITION + 5);
            putchar('|');
            gotoxy(X_POSITION + 10, Y_POSITION + 4);
            for (int i = 0; i < 10; i++)
            {
                putchar('=');
            }
            gotoxy(X_POSITION + 10, Y_POSITION + 6);
            for (int i = 0; i < 10; i++)
            {
                putchar('=');
            }
        }
    }

    gotoxy(0, Y_POSITION + 3);
    printf("X        ");
}

// Ve page Sign up
void drawSignUpPage(int sockfd)
{
    clearScreen();
    drawBorder();

    int X_POSITION = MARGIN_LEFT + 5 * WIDTH / 12,
        Y_POSITION = TOP + 4 * HEIGHT / 12;

    // danh sach cac lua chon
    printf(KYEL);
    gotoxy(X_POSITION, Y_POSITION);
    printf("Username: ");
    gotoxy(X_POSITION, Y_POSITION + 2);
    printf("Password: ");
    printf(KMAG);
    gotoxy(X_POSITION, Y_POSITION + 5);
    printf("Sign up");
    gotoxy(X_POSITION + 12, Y_POSITION + 5);
    printf("Return");
    gotoxy(X_POSITION, Y_POSITION - 2);
    printf(KCYN);
    printf("SIGN UP");
    gotoxy(X_POSITION - 12, Y_POSITION + 8);
    printf(KGRN);
    printf("--Press up/down to switch your choice--");

    int choice = 0;

    printf(KYEL);
    gotoxy(X_POSITION - 4, Y_POSITION);
    printf("\u27A4");

    char username[20], password[20], mesg[50];
    int usernameLen = 0, passwordLen = 0;

    while (1)
    {
        // phat hien nhan phim
        if (kbhit())
        {
            // xoa con tro o vi tri cu

            // lay ma ascii cu phim vua nhan
            char key = getch();
            // up
            if (key == 65 || key == 66 || key == 67 || key == 68)
            {
                gotoxy(X_POSITION - 12, Y_POSITION + 10);
                printf("                                                ");
                drawPointerSignUpPage(key, &choice, X_POSITION, Y_POSITION);
            }
            // Enter
            else if (key == 10)
            {
                if (choice == 0 || choice == 1)
                {
                    drawPointerSignUpPage(key, &choice, X_POSITION, Y_POSITION);
                }
                else if (choice == 2) // Click vao SIGN UP
                {
                    // gotoxy(0, 0);
                    // printf("username: %s - password: %s", username, password);
                    gotoxy(X_POSITION - 12, Y_POSITION + 10);
                    printf(KRED);

                    if (usernameLen == 0 || passwordLen == 0)
                    {
                        if (usernameLen == 0)
                        {
                            printf("--Please enter username--");
                        }
                        else if (passwordLen == 0)
                        {
                            printf("--Please enter password--");
                        }
                    }
                    else
                    {
                        strcpy(mesg, username);
                        strcat(mesg, "|");
                        strcat(mesg, password);
                        addToken(mesg, REGISTER_SIGNAL);

                        if (send(sockfd, (void *)mesg, strlen(mesg), 0) < 0)
                        {
                            printf("error");
                        };
                        strcpy(mesg, "");

                        recv(sockfd, mesg, 50, 0);
                        mesg[4] = '\0';
                        int tokenTotal;
                        char **data = words(mesg, &tokenTotal, "|");
                        SignalState signalState = (data[tokenTotal - 1][0] - '0') * 10 + data[tokenTotal - 1][1] - '0';
                        // int result = data[0][0] - '0';

                        if (signalState == SUCCESS_SIGNAL)
                        {
                            printf(KGRN);
                            gotoxy(X_POSITION - 12, Y_POSITION + 10);
                            printf("--Sign up success! Press Enter to continue--");
                            while (1)
                            {
                                if (kbhit())
                                {
                                    char key3 = getch();
                                    if (key3 == 10)
                                    {
                                        state = MAIN_MENU;
                                        return;
                                    }
                                }
                            }
                        }
                        else
                        {
                            printf(KRED);
                            gotoxy(X_POSITION - 12, Y_POSITION + 10);
                            printf("--Username already exist--");
                        }
                    }
                }
                else if (choice == 3) // Click vao Return
                {
                    state = MAIN_MENU;
                    break;
                }
            }
            // Nhap username/password tu ban phim
            else
            {
                if (choice == 0)
                {
                    getInput(key, username, 8, &usernameLen, X_POSITION, Y_POSITION);
                }
                else if (choice == 1)
                {
                    getInput(key, password, 8, &passwordLen, X_POSITION, Y_POSITION + 2);
                }
            }
        }

        printf(KWHT);
        if (choice == 0)
        {
            gotoxy(X_POSITION + 10 + usernameLen, Y_POSITION);
            printf("      ");
            gotoxy(X_POSITION + 10 + usernameLen, Y_POSITION);
        }
        else if (choice == 1)
        {
            gotoxy(X_POSITION + 10 + passwordLen, Y_POSITION + 2);
            printf("      ");
            gotoxy(X_POSITION + 10 + passwordLen, Y_POSITION + 2);
        }
        else
        {
            gotoxy(0, 0);
            printf("      ");
            gotoxy(0, 0);
        }
    }
}

// Ve page play & rank (page sau khi login thanh cong)

void drawReadyPage(int sockfd)
{
    system("clear");
    drawBorder();

    int X_POSITION = MARGIN_LEFT + 5 * WIDTH / 12,
        Y_POSITION = TOP + 5 * HEIGHT / 12;

    // danh sach cac lua chon
    printf(KWHT);
    gotoxy(X_POSITION, Y_POSITION);
    printf("Play / Watch");
    gotoxy(X_POSITION, Y_POSITION + 2);
    printf("Rank");
    gotoxy(X_POSITION, Y_POSITION + 4);
    printf("Log out");

    int pointer = Y_POSITION, choice;
    printf(KYEL);

    while (1)
    {
        sleep(0.3);
        // phat hien nhan phim
        if (kbhit())
        {
            // xoa con tro o vi tri cu

            gotoxy(X_POSITION - 10, pointer);
            putchar(' ');
            gotoxy(X_POSITION + 20, pointer);
            putchar(' ');
            gotoxy(X_POSITION - 10, pointer - 1);
            for (int i = 0; i < 31; i++)
            {
                putchar(' ');
            }
            gotoxy(X_POSITION - 10, pointer + 1);
            for (int i = 0; i < 31; i++)
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
            else if (key == 66)
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
            else if (key == 10)
            {
                choice = (pointer - Y_POSITION) / 2;
                if (choice == 0) // Lua chon vao page play/watch
                {
                    drawCommentPane();
                    system("clear");
                    drawBorder();

                    // danh sach cac lua chon
                    printf(KWHT);
                    gotoxy(X_POSITION, Y_POSITION);
                    printf("Play");
                    gotoxy(X_POSITION, Y_POSITION + 2);
                    printf("Rank");
                    gotoxy(X_POSITION, Y_POSITION + 4);
                    printf("Log out");
                }
                else if (choice == 1) // Lua chon xem page rank
                {
                    drawRank();
                    system("clear");
                    drawBorder();

                    // danh sach cac lua chon
                    printf(KWHT);
                    gotoxy(X_POSITION, Y_POSITION);
                    printf("Play");
                    gotoxy(X_POSITION, Y_POSITION + 2);
                    printf("Rank");
                    gotoxy(X_POSITION, Y_POSITION + 4);
                    printf("Log out");
                }
                else
                {
                    if (send(sockfd, (void *)"1", 2, 0) < 0)
                    {
                        gotoxy(0, 0);
                        printf("error");
                    };
                    state = MAIN_MENU;
                    return;
                }
            }
        }

        printf(KYEL);

        gotoxy(X_POSITION - 10, pointer);
        putchar('|');
        gotoxy(X_POSITION + 20, pointer);
        putchar('|');
        gotoxy(X_POSITION - 10, pointer - 1);
        for (int i = 0; i < 31; i++)
        {
            putchar('~');
        }
        gotoxy(X_POSITION - 10, pointer + 1);
        for (int i = 0; i < 31; i++)
        {
            putchar('~');
        }

        gotoxy(0, 0);
        printf(KWHT);
        printf("      ");
        gotoxy(0, 0);
    }
}

// typedef struct
// {
//     char name[10];
//     int win, loss, score, rank;
// } User;

// User createUser(char *name, int win, int loss, int score, int rank){
//     User user;
//     strcpy(user.name, name);
//     user.win = win;
//     user.loss = loss;
//     user.score = score;
//     user.rank = rank;
//     return user;
// }

// // In ra rank cua 1 user
// void printRank(User user, int X_POSITION, int Y_POSITION)
// {
//     gotoxy(X_POSITION + 2, Y_POSITION);
//     printf("%s", user.name);
//     gotoxy(X_POSITION + 14, Y_POSITION);
//     printf("%d", user.win);
//     gotoxy(X_POSITION + 23, Y_POSITION);
//     printf("%d", user.loss);
//     gotoxy(X_POSITION + 32, Y_POSITION);
//     printf("%d", user.score);
//     gotoxy(X_POSITION + 41, Y_POSITION);
//     printf("%d", user.rank);
// }

void drawSword(char *str, int x_position, int y_position)
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
            char c = str[i];
            if (c == ' ')
            {
                gotoxy(x_position + colNum + 1, y_position + lineNum);
            }
            else
            {
                if (c == 'S' || c == '8' || c == 'o')
                {
                    printf(KRED);
                    putchar(c);
                    printf(KWHT);
                }
                else if (c == '|' && (str[i + 1] == 'S' || str[i - 1] == 'S'))
                {
                    printf(KYEL);
                    putchar(c);
                }
                else if (c == 'V' || c == '/' || c == '\\' || c == '|')
                    {
                        printf(KWHT);
                        putchar(c);
                    }
                else if (c == ';')
                {
                    printf(KCYN);
                    putchar(c);
                }
                else
                {
                    printf(KYEL);
                    putchar(c);
                }
            }
            colNum++;
        }
    }
}

void drawShield(char *str, int x_position, int y_position)
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
            char c = str[i];
            if (c == ' ')
            {
                gotoxy(x_position + colNum + 1, y_position + lineNum);
            }
            else
            {
                if (c == '<' || c == '>' || c == 'o' || c == 'W')
                {
                    printf(KRED);
                    putchar(c);
                    printf(KWHT);
                }
                else if (c == '(' || c == ')' || c == '_' || c == '-')
                {
                    printf(KWHT);
                    putchar(c);
                }
                else if (c == '.' || c == '@')
                {
                    printf(KCYN);
                    putchar(c);
                }
                else
                {
                    printf(KYEL);
                    putchar(c);
                }
            }
            colNum++;
        }
    }
}

void drawRank()
{
    system("clear");
    drawBorder();

    int X_POSITION = MARGIN_LEFT + 3 * WIDTH / 12,
        Y_POSITION = 6;

    char *str_sword = readFile("./text/sword.txt");
    str_sword[strlen(str_sword)] = '\0';
    drawSword(str_sword, 14, 5);

    char *str_shield = readFile("./text/shield.txt");
    // str_shield[strlen(str_shield)] = '\0';
    drawShield(str_shield, 105, 5);

    printf(KYEL);
    gotoxy(X_POSITION + 2, Y_POSITION + 1);
    printf("Name");
    gotoxy(X_POSITION + 14, Y_POSITION + 1);
    printf("Win");
    gotoxy(X_POSITION + 23, Y_POSITION + 1);
    printf("Loss");
    gotoxy(X_POSITION + 32, Y_POSITION + 1);
    printf("Score");
    gotoxy(X_POSITION + 41, Y_POSITION + 1);
    printf("Rank");

    for (int i = 0; i < 3; i++)
    {
        printf(KCYN);
        for (int j = 0; j < 2; j++)
        {
            gotoxy(X_POSITION, Y_POSITION + i * 2 + j);
            putchar(220);
            gotoxy(X_POSITION + 12, Y_POSITION + i * 2 + j);
            putchar(220);
            gotoxy(X_POSITION + 21, Y_POSITION + i * 2 + j);
            putchar(220);
            gotoxy(X_POSITION + 30, Y_POSITION + i * 2 + j);
            putchar(220);
            gotoxy(X_POSITION + 39, Y_POSITION + i * 2 + j);
            putchar(220);
            gotoxy(X_POSITION + 48, Y_POSITION + i * 2 + j);
            putchar(220);
        }
        printf(KWHT);
        gotoxy(X_POSITION, Y_POSITION + i * 2);
        for (int j = 0; j < 49; j++)
        {
            putchar('_');
        }
    }

    while (1)
    {
        gotoxy(0, 0);
        printf("      ");
        gotoxy(0, 0);
        sleep(0.3);
        // phat hien nhan phim
        if (kbhit())
        {
            char key = getch();
            if (key == 10)
            {
                return;
            }
        }
    }
}

void getDetailReact(int choice, int X_POSITION, int Y_POSITION)
{
    if (choice == 0) // Ko tro vao icon nao ca
    {
        printf(KBLU);
        gotoxy(X_POSITION + 9, Y_POSITION + 1);
        printf("like");
        gotoxy(X_POSITION + 22, Y_POSITION + 1);
        printf("    ");
        gotoxy(X_POSITION + 32, Y_POSITION + 1);
        printf("    ");
    }
    else if (choice == 1) // Tro vao icon like
    {
        gotoxy(X_POSITION + 9, Y_POSITION + 1);
        printf("    ");
        printf(KYEL);
        gotoxy(X_POSITION + 22, Y_POSITION + 1);
        printf("haha");
        gotoxy(X_POSITION + 32, Y_POSITION + 1);
        printf("    ");
    }
    else if (choice == 2) // Tro vao icon haha
    {
        gotoxy(X_POSITION + 9, Y_POSITION + 1);
        printf("    ");
        gotoxy(X_POSITION + 22, Y_POSITION + 1);
        printf("    ");
        printf(KRED);
        gotoxy(X_POSITION + 32, Y_POSITION + 1);
        printf("love");
    }
    else // Tro vao icon love
    {
        gotoxy(X_POSITION + 9, Y_POSITION + 1);
        printf("    ");
        gotoxy(X_POSITION + 22, Y_POSITION + 1);
        printf("    ");
        gotoxy(X_POSITION + 32, Y_POSITION + 1);
        printf("    ");
    }
}

void getComment(char key, char *str, int maxlen, int *index, int X_POSITION, int Y_POSITION)
{
    if (key == 127)
    {
        // del 1 ky tu
        if ((*index) > 0)
        {
            str[--(*index)] = '\0';
        }
    }
    if ((*index) < maxlen - 1)
    {

        if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') ||
            (key >= '0' && key <= '9') || key == ' ' || key == ',' || key == '.')
        {
            str[(*index)++] = key;
            str[(*index)] = '\0';

            printf(KWHT);
            gotoxy(X_POSITION, Y_POSITION);
            printf("%s", str);
        }
    }
}

/******************Ve comment Pane******************************/

char cmtList[40][40]; // String luu toan bo comment
int totalCmt = 0;     // So luong cmt

void printComment(int choice, int X_POSITION, int Y_POSITION)
{
    if (choice == 3)
    {
        gotoxy(X_POSITION + 14, Y_POSITION + 3);
        printf("                         ");
        gotoxy(X_POSITION + 5, Y_POSITION + 4);
        printf("                                  ");
    }
    for (int i = 0; i < 28; i++)
    {
        gotoxy(X_POSITION + 2, Y_POSITION - 2 - i);
        printf("                                     ");
    }

    int totalLine;
    if (totalCmt > 28)
    {
        totalLine = 28;
    }
    else
    {
        totalLine = totalCmt;
    }
    for (int i = 0; i < totalLine; i++)
    {
        gotoxy(X_POSITION + 2, Y_POSITION - 1 + i - totalLine);
        printf("%s", cmtList[i]);
    }
}

//
void drawCommentPane()
{

    system("clear");
    drawBorder();

    int X_POSITION = 3 * WIDTH / 4 - 5,
        Y_POSITION = HEIGHT - 8;

    // Ve khung Comment

    printf(KBLU);
    for (int i = 3; i < HEIGHT; i++)
    {
        gotoxy(X_POSITION, i);
        putchar('|');
    }
    gotoxy(X_POSITION + 1, Y_POSITION - 1);
    for (int i = 0; i < WIDTH - X_POSITION - 2; i++)
    {
        putchar('_');
    }

    // Ve lua chon icon
    // printf("\u2665 👍 U+1F44D 😂 \u27A4 ");
    gotoxy(X_POSITION + 6, Y_POSITION + 1);
    printf("👍"); // choice = 0

    gotoxy(X_POSITION + 18, Y_POSITION + 1);
    printf("😂"); // choice = 1

    gotoxy(X_POSITION + 30, Y_POSITION + 1);
    printf(KRED);
    printf("\u2665"); // choice = 2

    // Ve lua chon comment
    printf(KCYN);
    gotoxy(X_POSITION + 5, Y_POSITION + 3);
    printf("Comment: "); // choice = 3

    // Ve lua chon dat cuoc
    printf(KRED);
    gotoxy(X_POSITION + 5, Y_POSITION + 5);
    printf("BET"); // choice = 4

    // Ve lua chon dat cuoc
    printf(KYEL);
    gotoxy(X_POSITION + 20, Y_POSITION + 5);
    printf("EXIT"); // choice = 4

    // In con tro
    gotoxy(X_POSITION + 2, Y_POSITION + 3);
    printf(KYEL);
    printf("\u27A4");

    printf(KWHT);
    gotoxy(X_POSITION + 14, Y_POSITION + 3);
    printf("      ");
    gotoxy(X_POSITION + 14, Y_POSITION + 3);

    int choice = 3; // Pointer dang o vi tri comment
    int pointer_x = X_POSITION + 2, pointer_y = Y_POSITION + 3;

    char username[10] = "long";

    char commentStr[30] = ""; // String luu comment nguoi dung dang dien vao
    int cmtLen = 0;           // Do dai comment cua commentStr

    while (1)
    {
        sleep(0.3);
        // phat hien nhan phim
        if (kbhit())
        {
            // lay ma ascii cu phim vua nhan
            char key = getch();
            // up
            if (key == 65 || key == 66 || key == 67 || key == 68)
            {

                // xoa con tro o vi tri cu
                gotoxy(pointer_x, pointer_y);
                printf("  ");

                if (key == 65 || key == 68)
                {
                    if (choice == 0)
                    {
                        choice = 5;
                        pointer_x = X_POSITION + 17;
                        pointer_y = Y_POSITION + 5;
                    }
                    else if (choice == 1)
                    {
                        choice = 0;
                        pointer_x = X_POSITION + 2;
                    }
                    else if (choice == 2)
                    {
                        choice = 1;
                        pointer_x = X_POSITION + 15;
                    }
                    else if (choice == 3)
                    {
                        choice = 2;
                        pointer_x = X_POSITION + 27;
                        pointer_y = Y_POSITION + 1;
                    }
                    else if (choice == 4)
                    {
                        choice = 3;
                        pointer_y = Y_POSITION + 3;
                    }
                    else
                    {
                        choice = 4;
                        pointer_x = X_POSITION + 2;
                    }
                }
                // down
                else if (key == 66 || key == 67)
                {
                    if (choice == 0)
                    {
                        choice = 1;
                        pointer_x = X_POSITION + 15;
                    }
                    else if (choice == 1)
                    {
                        choice = 2;
                        pointer_x = X_POSITION + 27;
                    }
                    else if (choice == 2)
                    {
                        choice = 3;
                        pointer_x = X_POSITION + 2;
                        pointer_y = Y_POSITION + 3;
                    }
                    else if (choice == 3)
                    {
                        choice = 4;
                        pointer_y = Y_POSITION + 5;
                    }
                    else if (choice == 4)
                    {
                        choice = 5;
                        pointer_x = X_POSITION + 17;
                    }
                    else
                    {
                        choice = 0;
                        pointer_x = X_POSITION + 3;
                        pointer_y = Y_POSITION + 1;
                    }
                }

                printf(KYEL);
                gotoxy(pointer_x, pointer_y);
                printf("\u27A4");
                getDetailReact(choice, X_POSITION, Y_POSITION);
            }
            else if (key == 10) // Nhan enter
            {
                if (choice != 4 && choice != 5)
                {
                    // printf("\u2665 👍 U+1F44D 😂 \u27A4 ");
                    if (choice != 3)
                    {
                        strcpy(cmtList[totalCmt], KCYN);
                        strcat(cmtList[totalCmt], username);
                        strcat(cmtList[totalCmt], ": ");
                        if (choice == 0)
                        {
                            strcat(cmtList[totalCmt], "  👍 ");
                        }
                        else if (choice == 1)
                        {
                            strcat(cmtList[totalCmt], "  😂  ");
                        }
                        else
                        {
                            strcat(cmtList[totalCmt], "  \x1B[31m\u2665  ");
                        }
                        totalCmt++;
                        printComment(1, X_POSITION, Y_POSITION);
                    }
                    else if (cmtLen != 0) // Lua chon post comment neu strlen(comment) > 0
                    {
                        strcpy(cmtList[totalCmt], KCYN);
                        strcat(cmtList[totalCmt], username);
                        strcat(cmtList[totalCmt], ": ");
                        strcat(cmtList[totalCmt], KWHT);
                        strcat(cmtList[totalCmt], commentStr);

                        totalCmt++;
                        strcpy(commentStr, "");
                        cmtLen = 0;
                        printComment(3, X_POSITION, Y_POSITION);
                    }
                }
                else if (choice == 4) // Lua chon BET
                {
                    return;
                }
                else // Lua chon EXIT
                {
                    return;
                }
            }
            else
            {
                if (choice == 3)
                {
                    getComment(key, commentStr, 24, &cmtLen, X_POSITION + 14, Y_POSITION + 3);
                }
            }

            printf(KWHT);
            if (choice == 3)
            {
                gotoxy(X_POSITION + 14 + cmtLen, Y_POSITION + 3);
                printf("    ");
                gotoxy(WIDTH - 1, Y_POSITION + 3);
                printf(KYEL);
                putchar('X');
                printf(KWHT);
                gotoxy(X_POSITION + 14 + cmtLen, Y_POSITION + 3);
            }
            else
            {
                gotoxy(0, 0);
                printf("      ");
                gotoxy(0, 0);
            }
        }
    }
}