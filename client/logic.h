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

#endif