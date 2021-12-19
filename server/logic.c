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
  // message|signal
  // Login successfully|SUCCESS_SIGNAL
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
    newU->next = head;
    saveToFile(newU, "account.txt");
    answer(confd, "Wellcome", SUCCESS_SIGNAL);
    return newU;
  }
  return head;
}

void logOut(User head, int confd) {
  User user = head;
  while(user != NULL) {
    if (user->online == confd) {
      user->online = -1;
      break;
    }
    user = user->next;
  }
}


/* go menu and notify the warrior has given up */
void cancelRound(User head, User player1, User player2) {
  User temp = head;
  while(temp != NULL) {
    if (temp->online > -1) {
      answer(temp->online, "The warrior has given up", MENU_SIGNAL);
    }
    temp->hp = -1;
    temp = temp->next;
  }
  player1 = NULL;
  player2 = NULL;
}


void disconnect(User head, User user1, User user2, int confd) {
  User user = head;
  while(user != NULL) {
    if (user->online == confd) {
      user->online = -1;
      break;
    } 
  }
  close(confd);

  if ((user1 != NULL && user1->online == confd) || (user2 != NULL && user2->online == confd)) {
    cancelRound(head, user1, user2);
    return;
  }

}



// ingame

void gameResult(User head, User winner, User losser) {
  char *message = "Winner: ";
  strcat(message, winner->username);

  User temp = head;
  while(temp != NULL) {
    if (temp->online > -1) {
      answer(temp->online, message, MENU_SIGNAL);
      temp->hp = -1;
    }
    temp = temp->next;
  }
  winner = NULL;
  losser = NULL;
}

void attack(User head, User attacker, User beingAttacked, int dame) {
  if (attacker == NULL || beingAttacked != NULL) {
    if (attacker == NULL) attacker->loss++;
    if (beingAttacked == NULL) beingAttacked->loss++;
    cancelRound(head, attacker, beingAttacked);
    return;
  }

  answer(attacker->online, "You attack", ATTACK_SIGNAL);
  answer(beingAttacked->online, "You are attacked", ATTACKED_SIGNAL);
  // stream(head, attacker, beingAttacked, dame);

  beingAttacked->hp -= dame;
  if (beingAttacked->hp <= 0) {
    attacker->win++;
    beingAttacked->loss++;
    gameResult(head, attacker, beingAttacked);
  }
}

User player(User head, int confd) {
  User user = findById(head, confd);
  user->hp = 1000;
  answer(confd, "Your are a player", SUCCESS_SIGNAL);
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
    answer(user2->online, "Let's go", SUCCESS_SIGNAL);
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