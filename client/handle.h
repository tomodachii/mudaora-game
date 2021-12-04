#ifndef __HANDLE_H__
#define __HANDLE_H__

typedef enum {
  LOGIN,
  LOGOUT,
  REGISTER,
} SignalState;

typedef struct {
  char username[20];
  char password[20];
  int win;
  int loss;
  int rank;
} User; 

char **words(char *line, int *total, char *strCut);
int isNumber(char *str);
int isIpV4(char *str);
void clearBuffer();
void addToken(char *str, SignalState us);

#endif