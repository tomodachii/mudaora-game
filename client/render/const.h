
#define WIDTH 140
#define HEIGHT 40
#define TOP 4
#define MARGIN_LEFT 6
#define SIZE 1050

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

enum state
{
    MAIN_MENU,
    LOGIN,
    SIGN_UP,
    QUIT
} state;