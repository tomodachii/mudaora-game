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
int setup(User A[], char *fileName);
User findUserName(User A[], int total, char *name);
int findIndex(User A[], int total, char *name);
void writeFile(User A[], int total, char *fileName);
// share memory between prossess
void* create_shared_memory(size_t size);

#endif