#ifndef __LOGIC_H__
#define __LOGIC_H__

typedef enum {
  LOGIN_SIGNAL,
  LOGOUT_SIGNAL,
  REGISTER_SIGNAL,
  DISCONNECT_SIGNAL,
  GET_RANK_SIGNAL,
  PLAYER_SIGNAL,
  MODE_SIGNAL,

  ATTACK_SIGNAL,
  ATTACKED_SIGNAL,
  YELL_SIGNAL,

  MENU_SIGNAL,
  SUCCESS_SIGNAL,
  FAILED_SIGNAL,
  ACCOUNT_NOT_FOUND_SIGNAL,
  PASSWORD_WRONG,
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