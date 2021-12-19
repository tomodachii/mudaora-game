#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "scrState.h"
#include "customWindow.h"
#include "logic.h"
#include "handle.h"

WINDOW *create_newWin(int row, int col, int startR, int startC, int borderSize) {
  WINDOW *local_win;
  local_win = newwin(row, col, startR, startC);
  // set border-style
  wattron(local_win, COLOR_PAIR(1));
  switch(borderSize) {
    case 0: {
      break;
    }
    case 1: {
      box(local_win, 0, 0);
      break;
    }
    case 2: {
      wborder(local_win, 'X', 'X', 'x', 'x', 'x', 'x', 'X', 'X');
      break;
    }
    default: {
    }
  }
  wrefresh(local_win);
  wattroff(local_win, COLOR_PAIR(1));
  return local_win;
}

void destroy_win(WINDOW *local_win) {
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  werase(local_win);
  wrefresh(local_win);
  delwin(local_win);
}

int menu(WINDOW *local_win, char list[][50], int total, int activeColor, int mediumOfItem) {
  // hidden cursor
  curs_set(0);
  // don't show keypress on screen
  noecho();
  WINDOW *wins[total];
  int i, row, col, select = 0;
  getmaxyx(local_win, row, col);

  for (i = 0; i < total; i++) {
    wins[i] = derwin(local_win, row/total, col, i*row/total, 0);
    int y = getmaxy(wins[i]);
    mvwprintw(wins[i], y/2, (col-mediumOfItem)/2, "%s", list[i]);
    wrefresh(wins[i]);
  }

  wattron(wins[select], COLOR_PAIR(activeColor));
  box(wins[select], 0, 0);
  wrefresh(wins[select]);

  int c;
  while(1) {
    c = getch();

    wattron(wins[select], COLOR_PAIR(8));
    box(wins[select], 0, 0);
    wrefresh(wins[select]);
    
    switch(c) {
      case KEY_UP: {
        if (select == 0) {
          select = total-1;
        } else {
          select--;
        }
        break;
      }
      case KEY_DOWN: {
        if (select == total-1) {
          select = 0;
        } else {
          select++;
        }
        break;
      }
      case 10: {
        if (select == 0 || select == 1) {
          for (i = 0; i < total; i++) {
            destroy_win(wins[i]);
          }
        }
        return select;
      }
      default: {}
    }
    wattron(wins[select], COLOR_PAIR(1));
    box(wins[select], 0, 0);
    wrefresh(wins[select]);
  }
  return -1;
}

// return login/register/quit
ScreenState menu_window(WINDOW *local_win, char *notify) {
  int row, col, itemTotal = 3;
  char menuItems[][50] = {
    " Login  ", 
    "Register",
    "  Quit  "
  };
  getmaxyx(local_win,row, col);

  int height = (itemTotal+1)*3; // +1 is a line that print message from server
  WINDOW *content_area = derwin(local_win, height, col, (row-height)/2, 0);
  // print message from server
  mvwprintw(content_area, 1, (col-strlen(notify))/2, "%s", notify);
  wrefresh(content_area);
  // show menu
  WINDOW *menu_area = derwin(content_area, height-3, col, 3, 0);
  int choose = menu(menu_area, menuItems, itemTotal, 1, 8);
  destroy_win(content_area);
  destroy_win(menu_area);
  if (choose == 0) return LOGIN;
  if (choose == 1) return REGISTER;
  return QUIT;
}

ScreenState select_mode_window(WINDOW *local_win) {
  werase(local_win);
  wrefresh(local_win);
  int row, col, itemTotal = 3;
  char menuItems[][50] = {
    "⚡ King of speed  ⚡",
    "💪 King of strength 💪",
    "Back"
  };
  getmaxyx(local_win,row, col);

  int height = itemTotal*3; 
  // WINDOW *content_area = derwin(local_win, height, col, (row-height)/2, 0);
  // show menu
  WINDOW *menu_area = derwin(local_win, height, col, (row-height)/2, 0);
  int choose = menu(menu_area, menuItems, itemTotal, 1, 20);
  // destroy_win(content_area);
  destroy_win(menu_area);
  if (choose == 0) return SPEED;
  if (choose == 1) return STRENGTH;
  return BACK;
}

ScreenState inputLogin(WINDOW *local_win, char outUsername[], char outPassword[]) {
  char loginForm[4][20] = {
    "Username:\t", 
    "Password:\t", 
    " Back ",
    " Login ", 
  };
  int row, col, currChoose = 0;
  char dot[20] = "";

  curs_set(0);
  raw();
  noecho();

  getmaxyx(local_win, row, col);

  WINDOW *content = derwin(local_win, 11, col, (row-11)/2, 0);
  wattron(content, COLOR_PAIR(1));
  box(content, 0,0);
  wattron(content, COLOR_PAIR(4));

  wattron(content, A_BOLD);
  mvwprintw(content, 0, 3, " Login ");
  wattroff(content, A_BOLD);
  
  int rows = 3, cols = col/2 - 18;
  while(1) {
    wattron(content, COLOR_PAIR(1));
    mvwprintw(content, rows + 1-currChoose, cols-5, "%s", " ");
    if (currChoose >= 2) {
      mvwprintw(content, rows, cols-5, "%s", " ");
      mvwprintw(content, rows+1, cols-5, "%s", " ");
    }
    if (currChoose < 2) {
      wattron(content, A_BLINK);
      mvwprintw(content, rows + currChoose, cols-5, "%s", "👉");
      wattroff(content, A_BLINK);
    }
    // change color for each text
    mvwprintw(content, rows, cols, "%s", loginForm[0]);
    wattron(content, COLOR_PAIR(4));
    wprintw(content, "%s", outUsername);
    wprintw(content, "%c", ' ');
    wattron(content, COLOR_PAIR(1));
    mvwprintw(content, rows + 1, cols, "%s", loginForm[1]);
    wattron(content, COLOR_PAIR(4));
    wprintw(content, "%s", dot);
    wprintw(content, "%c", ' ');
    wattron(content, COLOR_PAIR(1));


    if (currChoose == 2) {
      wattroff(content, COLOR_PAIR(1));
      wattron(content, COLOR_PAIR(3));
    }
    mvwprintw(content, rows + 3, cols, "%s", loginForm[2]);
    if (currChoose == 2) {
      wattroff(content, COLOR_PAIR(3));
      wattron(content, COLOR_PAIR(1));
    }

    if (currChoose == 3) {
      wattroff(content, COLOR_PAIR(1));
      wattron(content, COLOR_PAIR(2));
    }
    mvwprintw(content, rows + 3, cols+9, "%s", loginForm[3]);
    if (currChoose == 3) {
      wattroff(content, COLOR_PAIR(2));
      wattron(content, COLOR_PAIR(1));
    }

    wattroff(content, COLOR_PAIR(1));

    wrefresh(content);

    int keyCode = getch();
    // printw("%d", keyCode);
    switch(keyCode) {
      case KEY_UP: {
        if (currChoose == 3) {
          currChoose = 1;
          break;
        }
        currChoose--;
        if (currChoose == -1) currChoose = 0;
        break;
      }
      case KEY_DOWN: {
        currChoose++;
        if (currChoose > 3) currChoose = 0;
        break;
      }
      case KEY_LEFT: {
        if (currChoose == 2) {
          currChoose = 3;
        } else if (currChoose == 3) {
          currChoose = 2;
        }
        break;
      }
      case KEY_RIGHT: {
        if (currChoose == 2) {
          currChoose = 3;
        } else if (currChoose == 3) {
          currChoose = 2;
        }
        break;
      }
      case 10: {
        if (currChoose == 3) {
          char *str = "";            
          if (strlen(outUsername) == 0) {
            str = " Username is required !!! ";
          } else if (strlen(outPassword) == 0) {
            str = " Password is required !!! ";
          } else {
            // not error then username and pass are ok!
            destroy_win(content);
            return LOGIN;
          }
          wattron(content, COLOR_PAIR(7));
          mvwprintw(content, 9, (col-strlen(str))/2, "%s", str);
          wattroff(content, COLOR_PAIR(7));
          wrefresh(content);
        } else if (currChoose == 2) {
          // back and clear username, pass because when user input username or pass and choose back
          destroy_win(content);
          return -1;
        }
        break;
      }
      case KEY_BACKSPACE:
      case 127: {
        if (currChoose == 0) {
          if (strlen(outUsername) == 0) break;
          outUsername[strlen(outUsername)-1] = '\0';
        } else if (currChoose == 1) {
          if (strlen(outPassword) == 0) break;
          // don't show password, only show dots (*)
          outPassword[strlen(outPassword)-1] = '\0';
          dot[strlen(dot)-1] = '\0';
        }
        break;
      }
      default: {
        // only allowed a-zA-Z0-9, max of length are 12 characters
        if (
          (keyCode >= 'a' && keyCode <= 'z') ||
          (keyCode >= 'A' && keyCode <= 'Z') ||
          (keyCode >= '0' && keyCode <= '9')
        ) {
          if (currChoose == 0) {
            if (strlen(outUsername) > 12) break;
            outUsername[strlen(outUsername)] = keyCode;
            outUsername[strlen(outUsername)] = '\0';
          } else if (currChoose == 1) {
            if (strlen(outPassword) > 12) break;
            outPassword[strlen(outPassword)] = keyCode;
            dot[strlen(dot)] = '*';
            outPassword[strlen(outPassword)] = '\0';
          }
        }
      }
    }
  }
  return -1;
}

ScreenState inputRegister(WINDOW *local_win, char outUsername[], char outPassword[]) {
  char loginForm[5][20] = {
    "Username:\t", 
    "Password:\t", 
    "Re-Password:\t", 
    " Back ",
    " Register "
  };
  int row, col, currChoose = 0, oldChoose = 0;
  char rePass[20] = "";
  char dot1[20] = "";
  char dot2[20] = "";
  box(local_win, 0,0);

  getmaxyx(local_win, row, col);
  curs_set(0);
  raw();
  noecho();

  int height = 12;

  WINDOW *content = derwin(local_win, height, col, (row-height)/2, 0);
  wattron(content, COLOR_PAIR(1));
  box(content, 0, 0);
  wattron(content, COLOR_PAIR(4));
  wattron(content, A_BOLD);
  mvwprintw(content, 0, 3, " Register ");
  wattroff(content, A_BOLD);
  
  int rows = 3, cols = col/2 - 18;
  while(1) {
    wattron(content, COLOR_PAIR(1));
    mvwprintw(content, rows + oldChoose, cols-5, "%s", " ");
    oldChoose = currChoose;
    if (currChoose >= 3) {
      mvwprintw(content, rows, cols-5, "%s", " ");
      mvwprintw(content, rows+1, cols-5, "%s", " ");
      mvwprintw(content, rows+2, cols-5, "%s", " ");
    }
    if (currChoose < 3) {
      wattron(content, A_BLINK);
      mvwprintw(content, rows + currChoose, cols-5, "%s", "👉");
      wattroff(content, A_BLINK);
    }
    // change color for each text
    // username
    mvwprintw(content, rows, cols, "%s", loginForm[0]);
    wattron(content, COLOR_PAIR(4));
    wprintw(content, "%s", outUsername);
    wprintw(content, "%c", ' ');
    // password
    wattron(content, COLOR_PAIR(1));
    mvwprintw(content, rows + 1, cols, "%s", loginForm[1]);
    wattron(content, COLOR_PAIR(4));
    wprintw(content, "%s", dot1);
    wprintw(content, "%c", ' ');
    // re-pass
    wattron(content, COLOR_PAIR(1));
    mvwprintw(content, rows + 2, cols, "%s", loginForm[2]);
    wattron(content, COLOR_PAIR(4));
    wprintw(content, "%s", dot2);
    wprintw(content, "%c", ' ');
    wattron(content, COLOR_PAIR(1));


    if (currChoose == 3) {
      wattroff(content, COLOR_PAIR(1));
      wattron(content, COLOR_PAIR(3));
    }
    mvwprintw(content, rows + 4, cols, "%s", loginForm[3]);
    if (currChoose == 3) {
      wattroff(content, COLOR_PAIR(3));
      wattron(content, COLOR_PAIR(1));
    }

    if (currChoose == 4) {
      wattroff(content, COLOR_PAIR(1));
      wattron(content, COLOR_PAIR(2));
    }
    mvwprintw(content, rows + 4, cols+9, "%s", loginForm[4]);
    if (currChoose == 4) {
      wattroff(content, COLOR_PAIR(2));
    }

    wattroff(content, COLOR_PAIR(1));

    wrefresh(content);

    int keyCode = getch();
    // printw("%d", keyCode);
    switch(keyCode) {
      case KEY_UP: {
        if (currChoose == 4) {
          currChoose = 2;
          break;
        }
        currChoose--;
        if (currChoose == -1) currChoose = 0;
        break;
      }
      case KEY_DOWN: {
        currChoose++;
        if (currChoose > 4) currChoose = 3;
        break;
      }
      case KEY_LEFT: {
        if (currChoose == 3) {
          currChoose = 4;
        } else if (currChoose == 4) {
          currChoose = 3;
        }
        break;
      }
      case KEY_RIGHT: {
        if (currChoose == 3) {
          currChoose = 4;
        } else if (currChoose == 4) {
          currChoose = 3;
        }
        break;
      }
      case 10: {
        // register is 4 :))
        if (currChoose == 4) {
          char *str = "";
          if (strlen(outUsername) == 0) {
            str = " User is required !!! ";
          } else if (strlen(outPassword) == 0) {
            str = " Password is required !!! ";
          } else if (strcmp(outPassword, rePass) != 0) {
            str = " Re-password not match !!! ";
          } else {
            // not error then username and pass, re-pass are ok!
            destroy_win(content);
            return REGISTER;
          }
          wattron(content, COLOR_PAIR(7));
          mvwprintw(content, height-2, (col-strlen(str))/2, "%s", str);
          wattroff(content, COLOR_PAIR(7));
          wrefresh(content);
        } else if (currChoose == 3) {
          // back 
          destroy_win(content);
          return -1;
        }
        break;
      }
      case KEY_BACKSPACE:
      case 127: {
        if (currChoose == 0) {
          if (strlen(outUsername) == 0) break;
          outUsername[strlen(outUsername)-1] = '\0';
        } else if (currChoose == 1) {
          if (strlen(outPassword) == 0) break;
          outPassword[strlen(outPassword)-1] = '\0';
          dot1[strlen(dot1)-1] = '\0';
        } else if (currChoose == 2) {
          if (strlen(rePass) == 0) break;
          rePass[strlen(rePass)-1] = '\0';
          dot2[strlen(dot2)-1] = '\0';
        }
        break;
      }
      default: {
        if (
          (keyCode >= 'a' && keyCode <= 'z') ||
          (keyCode >= 'A' && keyCode <= 'Z') ||
          (keyCode >= '0' && keyCode <= '9')
        ) {
          if (currChoose == 0) {
            if (strlen(outUsername) > 12) break;
            outUsername[strlen(outUsername)] = keyCode;
            outUsername[strlen(outUsername)] = '\0';
          } else if (currChoose == 1) {
            if (strlen(outPassword) > 12) break;
            outPassword[strlen(outPassword)] = keyCode;
            dot1[strlen(dot1)] = '*';
            outPassword[strlen(outPassword)] = '\0';
          } else if (currChoose == 2) {
            if (strlen(rePass) > 12) break;
            rePass[strlen(rePass)] = keyCode;
            dot2[strlen(dot2)] = '*';
            rePass[strlen(rePass)] = '\0';
          }
        }
      }
    }
  }
  return -1;
}

// select rank or fight or logout
ScreenState port_window(WINDOW *local_win, char *notify) {
  werase(local_win);
  wrefresh(local_win);
  int itemTotal = 4;
  char items[][30] = {
    "🤜  FIGHT 🤛",
    "👏  MEET  👏",
    "🏆  RANK  🏆",
    "↩️  LOGOUT ↩️"
  };
  WINDOW *wins[itemTotal];
  int i, row, col, select = 0;
  getmaxyx(local_win, row, col);
  // print message from server
  mvwprintw(local_win, 1, (col-strlen(notify))/2, "%s", notify);
  wrefresh(local_win);
  for (i = 0; i < itemTotal; i++) {
    wins[i] = derwin(local_win, (row-3)/itemTotal, col, i*(row-3)/itemTotal+3, 0);
    int y = getmaxy(wins[i]);
    mvwprintw(wins[i], 1, (col-10)/2, "%s", items[i]);
    wrefresh(wins[i]);
  }

  wattron(wins[select], COLOR_PAIR(1));
  box(wins[select], 0, 0);
  wrefresh(wins[select]);

  int c;
  while(1) {
    c = getch();

    wattron(wins[select], COLOR_PAIR(8));
    box(wins[select], 0, 0);
    wrefresh(wins[select]);
    
    switch(c) {
      case KEY_UP: {
        if (select == 0) {
          select = itemTotal-1;
        } else {
          select--;
        }
        break;
      }
      case KEY_DOWN: {
        if (select == itemTotal-1) {
          select = 0;
        } else {
          select++;
        }
        break;
      }
      case 10: {
        if (select == 0 || select == 1) {
          for (i = 0; i < itemTotal; i++) {
            destroy_win(wins[i]);
          }
        }
        if (select == 2) {
          return RANK;
        } else if (select == 0) {
          return FIGHT;
        } else if (select == 1) {
          return MEET;
        } if (select == 3) {
          return LOGOUT;
        }
      }
      default: {}
    }
    wattron(wins[select], COLOR_PAIR(1));
    box(wins[select], 0, 0);
    wrefresh(wins[select]);
  }
  return -1;
}

void rankUI(WINDOW *local_win, char *rankString) {
  int y, x;
  getmaxyx(local_win, y, x);
  WINDOW *rank_ui_head = derwin(local_win, 3, x, 0, 0);
  wattron(rank_ui_head, COLOR_PAIR(1));
  box(rank_ui_head, 0, 0);

  char *banner = "🎖️  🏆 🎖️";
  mvwprintw(rank_ui_head, 1, (x-7)/2, "%s", banner);
  wrefresh(rank_ui_head);

  WINDOW *rank_list = derwin(local_win, y-3, x, 3, 0);
  box(rank_list, 0, 0);
  
  int totalToken, i;
  char **data = words(rankString, &totalToken, "~");
  for (i = 0; i < totalToken; i++) {
    mvwprintw(rank_list, i+1, 2, "%s", data[i]);
  }
  wrefresh(rank_list);
  mvwprintw(local_win, y-1, 3, "%s", " ENTER TO RETURN ");
  wrefresh(local_win);
  // press enter to return
  while(getch() != 10);

  destroy_win(rank_list);
  destroy_win(rank_ui_head);
}

void battleUI(WINDOW *local_win) {
  wattron(local_win, COLOR_PAIR(5) | A_BOLD);
  mvwprintw(local_win, 0, 2, "%s", "  😈 😈 FIGHTING 😈 😈  ");
  wattroff(local_win, COLOR_PAIR(6));
  wrefresh(local_win);
}

void messagesUI(WINDOW *local_win) {
  wattron(local_win, COLOR_PAIR(6) | A_BOLD);
  mvwprintw(local_win, 0, 2, "%s", " Chat 🗣  ");
  wattroff(local_win, COLOR_PAIR(6));
  wrefresh(local_win);
}

void messageUI(WINDOW *local_win) {
  wattron(local_win, COLOR_PAIR(4));
  mvwprintw(local_win, 0, 2, "%s", " Your message 💬 ");
  wattroff(local_win, COLOR_PAIR(4));
  wrefresh(local_win);
}


void showMessages(WINDOW *local_win, char messages[][20], int total) {
  int i, height = getmaxy(local_win);
  for (i = 0; i < total; i++) {
    if (total >= 20) break;
    mvwprintw(local_win, height-i-1, 1, messages[i]);
  }
}

ScreenState inputMessage(WINDOW *local_win, char message[]) {
  int height, width;
  curs_set(1);
  getmaxyx(local_win, height, width);
  mvwprintw(local_win, height, 3, "%s", " F1 TO LEAVE ");
  wmove(local_win, height/2, 1);
  wrefresh(local_win);
  while(1) {
    int c = getch();
    // linit of message length
    if (strlen(message) >= width-3) {
      beep();
      continue;
    }
    if (c == 10) {
      if (strlen(message) == 0) continue;
      int i;
      wmove(local_win, height/2, 1);
      for (i = 0; i < width-2; i++) {
        wprintw(local_win, "%c", ' ');
      }
      wrefresh(local_win);
      break;
    } else if (c == KEY_BACKSPACE || c == 127) {
      message[strlen(message)-1] = '\0';
      if (strlen(message) == 0) beep();
    } else if (c == KEY_F(1)) {
      curs_set(0);
      return HOME;
    } else {
      message[strlen(message)] = c;
      message[strlen(message)] = '\0';
    }
    mvwprintw(local_win, height/2, 1, "%s", message);
    wprintw(local_win, " ");
    int curX, curY;
    getyx(local_win, curY, curX);
    wmove(local_win, curY, curX-1);
    wrefresh(local_win);
  }
  return 1;
}
