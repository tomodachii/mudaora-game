#ifndef __USER_H__
#define __USER_H__

typedef struct {
  char username[20],
  char password[20],
  int win,
  int loss,
  int rank
} User; 

#endif