#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "handle.h"
#include "linkedList.h"
#include "logic.h"

// login logout signup disconnect
void answer(int confd, char *username, char *message, SignalState signal) {
  Data *data = (Data*)malloc(sizeof(Data)*100);
  data->signal = signal;
  data->username = username;
  data->message = message;
  while(send(confd, (void*)data, sizeof((void*)data), 0) < 0);
}

void logIn(User head, int confd, char *username, char *password) {
  User user = findByName(head, username);
  if (user == NULL) {
    answer(confd, "", "User is not exist", FAILED);
  } else if (user->online > -1) {
    answer(confd, "", "Account is login in other client", FAILED);
  } else if (user->password == password) {
    user->online = confd; // id of socket connection
    answer(confd, username, "Login successfully", SUCCESS);
  } else {
    answer(confd, "", "Password is wrong", FAILED);
  }
}

void signUp(User head, int confd, char *username, char *password) {
  User user = findByName(head, username);
  if (user != NULL) {
    answer(confd, "", "Account is exist", FAILED);
  } else {
    answer(confd, username, "Wellcome", SUCCESS);
  }
}

void logOut(User head, int confd, char *username) {
  User user = findByName(head, username);
  if (user != NULL) user->online = 0;
}

void disconnect(User head, User user1, User user2, int confd, char *username) {
  if (user1->username == username || user2->username == username) {
    cancelRound(head, user1, user2);
    return;
  }

  User user = findByName(head, username);
  if (user != NULL) user->online = -1;
  close(confd);
}


// ingame

/* go menu and notify the warrior has given up */
void cancelRound(User head, User player1, User player2) {
  User temp = head;
  while(temp != NULL) {
    if (temp->online > -1) {
      answer(temp->online, "", "The warrior has given up", MENU);
    }
    temp->hp = -1;
    temp = temp->next;
  }
  player1 = NULL;
  player2 = NULL;
}

void gameResult(User head, User winner, User losser) {
  char *message = "Winner: ";
  strcat(message, winner->username);

  User temp = head;
  while(temp != NULL) {
    if (temp->online > -1) {
      answer(temp->online, "", message, MENU);
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

  answer(attacker->online, attacker->username, "You attack", ATTACK);
  answer(beingAttacked->online, beingAttacked->username, "You are attacked", ATTACKED);
  stream(head, attacker, beingAttacked, dame);

  beingAttacked->hp -= dame;
  if (beingAttacked->hp <= 0) {
    attacker->win++;
    beingAttacked->loss++;
    gameResult(head, attacker, beingAttacked);
  }
}

User player(User head, User player1, User player2, char *username) {
  User u = head, user = NULL;
  int playerTotal = 0;
  while(u != NULL) {
    if (u->username == username) user = u;
    if (u->hp > -1) {
      playerTotal ++;
    }
  }

  if (user != NULL) {
    if (playerTotal == 0) {
      user->hp = 1000;
      player1 = user;
    } else if (playerTotal == 1) {
      user->hp = 1000;
      player2 = user;
    }
  }
}

