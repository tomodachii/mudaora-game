#include <stdio.h>
#include <string.h>


int main() {
  char str[30] = "fjahdfa";
  
  strcpy(str, "");
  strcpy(str, "fd");
  printf("%s", str);
  return 0;
}