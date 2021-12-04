#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "handle.h"

/* divide the string to substrings by strCut
  total: total of substirngs
*/
char **words(char *line, int *total, char *strCut) {
  char **w = (char**)malloc(sizeof(char)*100);
  *w = (char*)malloc(sizeof(char)*100);

  int n = 0;
  char * token = strtok(line, strCut);
  while( token != NULL ) {
    *(w + n++)  = token;
    token = strtok(NULL, strCut);
  }
  *total = n;
  return w;
}

/* Check if string converts to number */
int isNumber(char *str) {
  int i, len = strlen(str);
  for (i = 0; i < len; i++) {
    if (str[i] < '0' || str[i] > '9') return 0;
  }
  return 1;
}

/* Check if string is ipv4
Ex:  127.0.0.1 */
int isIpV4(char *str) {
  char temp[100];
  strcpy(temp, str);
  int total;
  char **e = words(temp, &total, ".");

  if (total != 4) {
    return 0;
  };

  int i;
  for (i = 0; i < total; i++) {
    if (!isNumber(e[i]) || atoi(e[i]) < 0 || atoi(e[i]) > 255) return 0;
  }

  return 1;
}

void clearBuffer() {
  int c;
  do {
    c = getchar();
  } while (c != '\n' && c != EOF);
}

/* add a token to tail of string space by | */
void addToken(char *str, SignalState us) {
  int len = strlen(str);
  str[len] = '|';
  str[len+1] = '0' + us;
  str[len+2] = '\0';
}

int setup(User A[], char *fileName) {
  int i = 0;
  FILE *f = fopen(fileName, "r");
  if (f == NULL) {
    printf("File not found");
    exit(0);
  }

  while (!feof(f)) {
    char str[100];
    int total;
    fgets(str, 100, f);
    char **e = words(str, &total, "\t\n ");
    if (total != 4) continue;
    strcpy(A[i].username, e[0]);
    strcpy(A[i].password, e[1]);
    A[i].win = atoi(e[2]);
    A[i].loss = 3;
    i++;
  }
  fclose(f);
  return i;
}

User findUserName(User A[], int total, char *name) {
  User a;
  strcpy(a.username, "");
  strcpy(a.password, "");
  int i;
  for (i = 0; i < total; i++) {
    if (strcmp(A[i].username, name) == 0) {
      strcpy(a.username, A[i].username);
      strcpy(a.password, A[i].password);
      a.win = A[i].win;
      a.loss = A[i].loss;
      a.rank = A[i].rank;
    }
  }
  return a;
}

int findIndex(User A[], int total, char *name) {
  int i;
  for (i = 0; i < total; i++) {
    if (strcmp(A[i].username, name) == 0) {
      return i;
    }
  }
  return -1;
}

void writeFile(User A[], int total, char *fileName) {
  int i;
  FILE *f = fopen(fileName, "w");
  if (f == NULL) {
    printf("Write to file failed");
    exit(0);
  }
  for (i = 0; i < total; i++) {
    fprintf(f, "%s\t%s\t%d\n", A[i].username, A[i].password, A[i].win, A[i].loss);
  }
  fclose(f);
}
