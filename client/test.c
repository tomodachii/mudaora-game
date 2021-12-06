#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct USER {
  char *username;
  char *password;
  int win;
  int loss;
  int rank;
  int online;

  struct USER *next;
};

typedef struct USER *User;

User newUser(char *username, char *password) {
  User newUser = (User)malloc(sizeof(User)*100);
  newUser->username = username;
  newUser->password = password;
  newUser->win = 0;
  newUser->loss = 0;
  newUser->rank = 0;
  newUser->online = 0;
  newUser->next = NULL;
  return newUser;
}

User addHead(User head, User user) {
  if (head == NULL) return user;

  user->next = head;
  return user;
}


User findByName(User head, char *username) {
  User user = head;
  while (user != NULL) {
    if (user->username == username) return user;
    user = user->next;
  }
  return NULL;
}

User sortByRank(User head) {

}

int main() {
  User head = NULL;
  char *str = "bac";
  User u1 = newUser("bac", "123");
  User u2 = newUser("bien", "1238");
  head = addHead(head, u1);
  head = addHead(head, u2);
  // printf("%d", head->username == str);
  // User user = findByName(head, "bac");
  // user->username = "bacdz";
  // user->password = "456";
  puts(findByName(head, "bac")->password);
  return 0;
}