#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

WINDOW *test_win = NULL, *main_win = NULL; 
char str[100];

void *testThread() {
  pthread_detach(pthread_self());
  int n = 0;
  while(n <= 10000) {
    wprintw(test_win, "ok");
    wrefresh(test_win);
    n += 200;
    napms(200);
  }
  return NULL;
}

int main() {
  initscr();
  test_win = newwin(10, 10, 0, 10);
  box(test_win, 0, 0);
  wrefresh(test_win);
  main_win = newwin(10, 10, 0, 11);

  pthread_t threadID;
  pthread_create(&threadID, NULL, testThread, NULL);
  wgetstr(main_win, str);

  getch();
  printw("%s\n", str);
  getch();
  
  endwin();
}