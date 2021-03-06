#ifndef __LOGIC_H__
#define __LOGIC_H__

typedef enum {
  STRENGTH,
  SPEED
} Mode;

typedef enum {
  LOGIN_SIGNAL,
  REGISTER_SIGNAL,
  LOGOUT_SIGNAL,
  GET_RANK_SIGNAL,
  GET_INFO_CURR_GAME,
  CANCEL_MATCH,

  ATTACK_SIGNAL,
  ATTACKED_SIGNAL,
  YELL_SIGNAL,

  MENU_SIGNAL,
  SUCCESS_SIGNAL,
  FAILED_SIGNAL,
  SPEED_SIGNAL,
  STRENGTH_SIGNAL,
  GIVE_IN,
  BET,
  RESULT_SIGNAL,
  BET_P1,
  BET_P2, 
  LEAVE_STREAM, 
  JOIN_STREAM,
  ALLOW_ATTACK_SIGNAL
} SignalState;

void addToken(char *str, SignalState signal);
void answer(int confd, char *message, SignalState signal);
void logIn(User head, int confd, char *username, char *password);
User signUp(User head, int confd, char *username, char *password);
void logOut(User head, int confd);
void getRank(User head, int confd);
User player(User head, int confd);
void playerError(int confd);
void getInfoCurrGame(User head, User user1, int bet1, User user2, int bet2, int confd, int totalViewer);
void winLose(User head, User user1, User user2);
void yell(User head, char *message, int confd);
void bet(User head, User user1, int bet1, User user2, int bet2);
void leave_stream(User head);
void attack(User head, User player1, User player2, int dmg, int confd);
void allowAttack(User head, User player1, User player2);

#endif