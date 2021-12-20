#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

struct USER {
  char username[20];
  char password[20];
  int win;
  int loss;
  int rank;
  int online;
  int hp;
  struct USER *next;
};

typedef struct USER *User;

User newUser(char *username, char *password);
User readUser(char *username, char *password, int win, int loss);
User addHead(User head, User user);
User findByName(User head, char *username);
User findById(User head, int confd);
User sortByRank(User head);
User setup(char *fileName);
void saveToFile(User head, char *fileName);

#endif