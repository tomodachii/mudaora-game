#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void addToken(char *str, int signal) {
  // int len = strlen(str);
  // str[len] = '|';
  // str[len+1] = '0' + signal;
  // str[len+2] = '\0';
  strcat(str, "|");
  puts(str);
}

int main() {
  // int i = 100;
  // sprintf(str, "%d", 42);
  // char *str = (char*)&i;
  // printf("%s\n", str);
  char str[50] = "hello";
  addToken(str, 2);
  return 0;
}