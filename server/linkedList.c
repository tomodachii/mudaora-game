#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linkedList.h"
#include "handle.h"

User newUser(char *username, char *password) {
  User newUser = (User)malloc(sizeof(User)*100);
  newUser->username = username;
  newUser->password = password;
  newUser->win = 0;
  newUser->loss = 0;
  newUser->rank = 0;
  newUser->online = -1;
  newUser->hp = -1;
  newUser->next = NULL;
  return newUser;
}

User readUser(char *username, char *password, int win, int loss) {
  User newUser = (User)malloc(sizeof(User)*100);
  newUser->username = username;
  newUser->password = password;
  newUser->win = win;
  newUser->loss = loss;
  newUser->rank = win - loss;
  newUser->online = -1;
  newUser->hp = -1;
  newUser->next = NULL;
  return newUser;
}

User addHead(User head, User user) {
  if (head == NULL) return user;

  user->next = head;
  return user;
}

User findByName(User head, char *username) {
  User user = head;
  while (user != NULL) {
    if (user->username == username) return user;
    user = user->next;
  }
  return NULL;
}

User sortByRank(User head) {
  printf("nothing :))");
  return NULL;
}

User setup(char *fileName) {
  FILE *f = fopen(fileName, "r");
  if (f == NULL) {
    printf("File not found");
    exit(0);
  }

  User head = NULL;
  while (!feof(f)) {
    char str[100];
    int total;
    fgets(str, 100, f);
    char **e = words(str, &total, "\t\n ");
    if (total != 4) continue;

    User user = readUser(e[0], e[1], atoi(e[2]), atoi(e[3]));
    head = addHead(head, user);
  }
  fclose(f);
  return head;
}

void saveToFile(User head, char *fileName) {
  FILE *f = fopen(fileName, "w");
  if (f == NULL) {
    printf("Write to file failed");
    exit(0);
  }

  User user = head;
  while (user != NULL) {
    fprintf(f, "%s\t%s\t%d\t%d\n", user->username, user->password, user->win, user->loss);
    user = user->next;
  }
  fclose(f);
}