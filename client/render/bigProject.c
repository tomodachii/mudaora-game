#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>

#include "drawingFunc.h"
#include "const.h"

int main()
{
    state = 0; // state = MENU;
    drawMainMenu();
    while (1)
    {
        if (state == MAIN_MENU)
        {
            drawMainMenu();
        }
        if (state == LOGIN)
        {
            drawLoginPage(1);
        }
        if (state == SIGN_UP)
        {
            drawSignUpPage();
        }
        if (state == QUIT)
        {
            quit();
            break;
        }
        // sleep(0.5);
    }

    remove("data");

    return 0;
}