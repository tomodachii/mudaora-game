#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "handle.h"
#include "linkedList.h"
#include "logic.h"

void addToken(char *str, SignalState signal) {
  int len = strlen(str);
  str[len] = '|';
  str[len+1] = '0' + signal;
  str[len+2] = '\0';
}

// login logout signup disconnect
void answer(int confd, char *message, SignalState signal) {
  char string[1000] = "";
  strcat(string, message);
  addToken(string, signal);
  send(confd, string, strlen(string), 0);
}

void sendAllServer(User head, char *message, SignalState SIGNAL) {
  User user = head;
  while(user != NULL) {
    if (user->online > 0) {
      answer(user->online, message, SIGNAL);
    }
    user = user->next;
  }
}

void logIn(User head, int confd, char *username, char *password) {
  User user = findByName(head, username); 
  if (user == NULL) {
    answer(confd, "User is not exist", FAILED_SIGNAL);
  } else if (user->online > -1) {
    answer(confd, "Account is login in other client", FAILED_SIGNAL);
  } else if (strcmp(user->password, password) == 0) {
    user->online = confd; // id of socket connection
    answer(confd, "Login successfully", SUCCESS_SIGNAL);
  } else {
    answer(confd, "Password is wrong", FAILED_SIGNAL);
  }
}

User signUp(User head, int confd, char *username, char *password) {
  User user = findByName(head, username);
  if (user != NULL) {
    answer(confd, "Account is exist", FAILED_SIGNAL);
  } else {
    User newU = newUser(username, password);
    newU->online = confd;
    newU->next = head;
    saveToFile(newU, "account.txt");
    answer(confd, "Wellcome", SUCCESS_SIGNAL);
    return newU;
  }
  return head;
}

void logOut(User head, int confd) {
  User user = findById(head, confd);
  if(user != NULL) {
    user->online = -1;
  }
}

User player(User head, int confd) {
  User user = findById(head, confd);
  user->hp = 1000;
  return user;
}

void playerError(int confd) {
  answer(confd, "Not match", FAILED_SIGNAL);
}

void getInfoCurrGame(User head, User user1, User user2, int confd) {
  // player 1
  if (user1 != NULL && user1->online == confd && user2 == NULL) {
    return;
  }
  // player 2
  if (user1 != NULL && user2 != NULL && user2->online == confd) {
    answer(user1->online, "Let's go", SUCCESS_SIGNAL);
    answer(confd, "Let's go", SUCCESS_SIGNAL);
    return;
  }
  // viewer
  if (user1 != NULL && user2 != NULL) {
    answer(confd, "Let's go", SUCCESS_SIGNAL);
    return;
  }
  // is viewer but players is not 2
  answer(confd, "Not played", FAILED_SIGNAL);
}

void winLose(User head, User user1, User user2) {
  user1->win++;
  user2->loss++;

  char mess[100] = "Winner: ";
  strcat(mess, user1->username);
  strcat(mess, " - Loser: ");
  strcat(mess, user2->username);
  sendAllServer(head, mess, RESULT_SIGNAL);
}

void yell(User head, char *message, int confd) {
  User user = findById(head, confd);
  char str[100] = "";
  strcpy(str, user->username);
  strcat(str, "|");
  strcat(str, message);
  sendAllServer(head, str, YELL_SIGNAL);
  // printf("%s", message);
}