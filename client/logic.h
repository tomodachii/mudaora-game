#ifndef __LOGIC_H__
#define __LOGIC_H__

typedef enum {
  LOGIN,
  LOGOUT,
  REGISTER,
  SUCCESS,
  FAILED,
  MENU,
  ATTACK,
  ATTACKED,
} SignalState;

typedef enum {
  EXACTLY,
  SPEED
} Mode;

typedef struct {
  char *message;
  char *username;
  char *password;
  SignalState signal;
} Data;

#endif