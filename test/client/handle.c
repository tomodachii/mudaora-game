#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <termios.h>
#include <fcntl.h>

#include "logic.h"
#include "handle.h"


int kbhit(void) {
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

  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

int getchByHLone(void) {
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

/* divide the string to substrings by strCut
  total: total of substirngs
*/
char **words(char *line, int *total, char *strCut)
{
  char **w = (char **)malloc(sizeof(char) * 100);
  *w = (char *)malloc(sizeof(char) * 100);

  int n = 0;
  char *token = strtok(line, strCut);
  while (token != NULL)
  {
    *(w + n++) = token;
    token = strtok(NULL, strCut);
  }
  *total = n;
  return w;
}

/* Check if string converts to number */
int isNumber(char *str)
{
  int i, len = strlen(str);
  for (i = 0; i < len; i++)
  {
    if (str[i] < '0' || str[i] > '9')
      return 0;
  }
  return 1;
}

/* Check if string is ipv4
Ex:  127.0.0.1 */
int isIpV4(char *str)
{
  char temp[100];
  strcpy(temp, str);
  int total;
  char **e = words(temp, &total, ".");

  if (total != 4)
  {
    return 0;
  };

  int i;
  for (i = 0; i < total; i++)
  {
    if (!isNumber(e[i]) || atoi(e[i]) < 0 || atoi(e[i]) > 255)
      return 0;
  }

  return 1;
}

void clearBuffer()
{
  int c;
  do
  {
    c = getchar();
  } while (c != '\n' && c != EOF);
}

void addToken(char *str, SignalState signal)
{
  int len = strlen(str);
  str[len] = '|';
  str[len + 1] = '0' + signal;
  str[len + 2] = '\0';
}