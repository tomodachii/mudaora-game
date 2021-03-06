#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

struct USER {
  char username[20];
  char password[20];
  int win;
  int loss;
  int score;
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
User setup(char *fileName);
void saveToFile(User head, char *fileName);

void swap(User user1, User user2);
void getRankList(User head, char* rankList, char* loggedInUserName);
void sortList(User head);

#endif