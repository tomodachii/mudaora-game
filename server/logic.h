#ifndef __LOGIC_H__
#define __LOGIC_H__

typedef enum {
  LOGIN,
  LOGOUT,
  REGISTER,
  DISCONNECT,
  GET_RANK,
  PLAYER,
  MODE,

  ATTACK,
  ATTACKED,
  YELL,

  MENU,
  SUCCESS,
  FAILED,
  BET
} SignalState;

typedef enum {
  EXACTLY,
  SPEED
} Mode;

void addToken(char *str, SignalState signal);
void answer(int confd, char *message, SignalState signal);
void logIn(User head, int confd, char *username, char *password);
void signUp(User head, int confd, char *username, char *password);
void logOut(User head, int confd);
void cancelRound(User head, User player1, User player2);
void disconnect(User head, User user1, User user2, int confd);
void gameResult(User head, User winner, User losser);
void attack(User head, User attacker, User beingAttacked, int dame);
void player(User head, User player1, User player2, int confd);


#endif