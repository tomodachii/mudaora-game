#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linkedList.h"
#include "handle.h"

User newUser(char *username, char *password) {
  User newUser = (User)malloc(sizeof(User)*100);
  strcpy(newUser->username, username);
  strcpy(newUser->password, password);
  newUser->win = 0;
  newUser->loss = 0;
  newUser->score = 0;
  newUser->online = -1;
  newUser->hp = -1;
  newUser->next = NULL;
  return newUser;
}

User readUser(char *username, char *password, int win, int loss) {
  User newUser = (User)malloc(sizeof(User)*100);
  strcpy(newUser->username, username);
  strcpy(newUser->password, password);
  newUser->win = win;
  newUser->loss = loss;
  newUser->score = win*5 - loss*2;
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
    if (strcmp(user->username, username) == 0) return user;
    user = user->next;
  }
  return NULL;
}

User findById(User head, int confd) {
  User user = head;
  while (user != NULL) {
    if (user->online == confd) return user;
    user = user->next;
  }
  return NULL;
}

User setup(char *fileName) {
  User head = (User)malloc(sizeof(User)*100);
  FILE *f = fopen(fileName, "r");
  if (f == NULL) {
    printf("File not found");
    exit(0);
  }

  while (!feof(f)) {
    char str[100];
    fgets(str, 100, f);
    if (strlen(str) == 0) continue;
    int total;
    char **e = words(str, &total, "\t\n ");
    if (total != 4) continue;
    if (strlen(e[0]) == 0 || strlen(e[1]) == 0) continue;
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

  User p;
  for(p = head; p != NULL; p = p->next) {
    fprintf(f, "%s\t%s\t%d\t%d\n", p->username, p->password, p->win, p->loss);
  }
  fclose(f);
}

void swap(User user1, User user2) {
  char username[50];
  char password[50];
  int win, loss, score, online, hp;

  strcpy(username, user1->username);
  strcpy(user1->username, user2->username);
  strcpy(user2->username, username);

  strcpy(password, user1->password);
  strcpy(user1->password, user2->password);
  strcpy(user2->password, password);

  win = user1->win;
  user1->win = user2->win;
  user2->win = win;

  loss = user1->loss;
  user1->loss = user2->loss;
  user2->loss = loss;

  score = user1->score;
  user1->score = user2->score;
  user2->score = score;

  online = user1->online;
  user1->online = user2->online;
  user2->online = online;

  hp = user1->hp;
  user1->hp = user2->hp;
  user2->hp = hp;
}

void getRankList(User head, char* rankList, char* loggedInUserName) {
  User current = head;
  int userInTop = 0;
  char rank[3];
  char win[10];
  char lose[10];
  char score[10];
  int count = 1;

  while(current != NULL && count < 15) {
    if(strcmp(current->username, loggedInUserName) == 0) {
      userInTop = 1;
    }
    sprintf(win, "%d", current->win);
    sprintf(lose, "%d", current->loss);
    sprintf(score, "%d", current->score);
    sprintf(rank, "%d", count);

    strcat(rankList, current->username);
    if (strlen(current->username) < 6) {
      strcat(rankList, "\t");
    }
    strcat(rankList, "\t");
    strcat(rankList, win);
    strcat(rankList, "\t");
    strcat(rankList, lose);
    strcat(rankList, "\t");
    strcat(rankList, score);
    strcat(rankList, "\t");
    strcat(rankList, rank);
    strcat(rankList, "~");

    count++;
    current = current->next;
  }

  if(userInTop == 0) {
    while(current != NULL) {
      if(strcmp(current->username, loggedInUserName) == 0) {
        sprintf(win, "%d", current->win);
        sprintf(lose, "%d", current->loss);
        sprintf(score, "%d", current->score);
        sprintf(rank, "%d", count);
        
        if(count > 11) {
          strcat(rankList, "...~");
        }
        strcat(rankList, current->username);
        if (strlen(current->username) < 6) {
          strcat(rankList, "\t");
        }
        strcat(rankList, "\t");
        strcat(rankList, win);
        strcat(rankList, "\t");
        strcat(rankList, lose);
        strcat(rankList, "\t");
        strcat(rankList, score);
        strcat(rankList, "\t");
        strcat(rankList, rank);
        strcat(rankList, "~...~");
      }
      count++;
      current = current->next;
    }
  }
  // printf("\n%s\n", loggedInUserName);
}

void sortList(User head) {
  User current = head;
  User index = NULL;

  if (head == NULL) {
    return;
  }
  else {
    while(current != NULL) {
      index = current->next;  
        
      while(index != NULL) {  
        if(current->score < index->score) {
          swap(current, index);
        }
        index = index->next;
      }
      current = current->next;
    }
  }
}