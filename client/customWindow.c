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
  wattroff(local_win, COLOR_PAIR(1));
  return local_win;
}

void destroy_win(WINDOW *local_win) {
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  werase(local_win);
  wrefresh(local_win);
  delwin(local_win);
}

void eraseInline(WINDOW *local_win, int row, int colFrom, int colTo) {
  wmove(local_win, row, colFrom);
  int i;
  for (i = colFrom; i <= colTo; i++) {
    wprintw(local_win, "%c", ' ');
  }
  wmove(local_win, row, colFrom);
  wrefresh(local_win);
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
    "         Back         "
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
  char items[][50] = {
    "🤜         FIGHT         🤛",
    "👏  WATCH CURRENT MATCH  👏",
    "🏆         RANK          🏆",
    "↩️          LOGOUT         ↩️"
  };
  int col = getmaxx(local_win);
  // print message from server
  mvwprintw(local_win, 1, (col-strlen(notify))/2, "%s", notify);
  wrefresh(local_win);
  WINDOW *content_win = derwin(local_win, 12, col, 3, 0);
  int index = menu(content_win, items, itemTotal, 1, 26);
  if (index == 0) return FIGHT;
  if (index == 1) return MEET;
  if (index == 2) return RANK;
  return LOGOUT;
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

  wattron(rank_list, COLOR_PAIR(1));
  mvwprintw(rank_list, 1, 2, "%s\t\t%s\t%s\t%s\t%s", "Name", "Win", "Loss", "Score", "Rank");
  wmove(rank_list, 2, 1);
  wattron(rank_list, COLOR_PAIR(4));
  for(i = 0; i < x-2; i++) {
    wprintw(rank_list, "_");
  }
  for (i = 0; i < totalToken; i++) {
    if (i >= 3) {
      wattroff(rank_list, COLOR_PAIR(1));
    } else{
      wattron(rank_list, COLOR_PAIR(1));
    }
    mvwprintw(rank_list, i+3, 2, "%s", data[i]);
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
  wattron(local_win, COLOR_PAIR(1));
  box(local_win, 0, 0);
  wattron(local_win, COLOR_PAIR(5) | A_BOLD);
  mvwprintw(local_win, 0, 2, "%s", "  😈 😈 FIGHTING 😈 😈  ");
  wattroff(local_win, COLOR_PAIR(6));
  wrefresh(local_win);
}

void messagesUI(WINDOW *local_win) {
  wattron(local_win, COLOR_PAIR(1));
  box(local_win, 0, 0);
  wattron(local_win, COLOR_PAIR(6) | A_BOLD);
  mvwprintw(local_win, 0, 2, "%s", " Chat 🗣  ");
  wattroff(local_win, COLOR_PAIR(6));
  wrefresh(local_win);
}

void messageUI(WINDOW *local_win) {
  wattron(local_win, COLOR_PAIR(1));
  box(local_win, 0, 0);
  wattron(local_win, COLOR_PAIR(4));
  mvwprintw(local_win, 0, 2, "%s", " Your message 💬 ");
  wattroff(local_win, COLOR_PAIR(4));
  wrefresh(local_win);
}

void messageContentUI(WINDOW *local_win) {
  scrollok(local_win, TRUE);
  wrefresh(local_win);
}

void betUI(WINDOW *local_win, char *username1, char *username2) {
  wattron(local_win, COLOR_PAIR(1));
  box(local_win, 0, 0);
  char bet[20] = " BET ";
  int w = getmaxx(local_win);
  mvwprintw(local_win, 0, (w-strlen(bet))/2, "%s", bet);
  wattron(local_win, COLOR_PAIR(4));
  mvwprintw(local_win, 1, 1, "F1: %s\t/\tF2: %s", username1, username2);
  wrefresh(local_win);
}

void ratioUI(WINDOW *local_win, char *username1, int rate1, char *username2, int rate2) {
  werase(local_win);
  wmove(local_win, 0, 0);
  // info ex: username1:20 username2:50
  
  float user1Parsent = (float)rate1/(rate1+rate2)*100;
  float user2Parsent = 100-user1Parsent;
  
  int col = getmaxx(local_win);
  int user1len = user1Parsent/100*col;
  

  int i;
  wattron(local_win, COLOR_PAIR(7));
  for (i = 0; i < col; i++) {
    if (i > user1len) {
      wattron(local_win, COLOR_PAIR(11));
    }
    wprintw(local_win, " ");
  }

  wattron(local_win, COLOR_PAIR(4));
  mvwprintw(local_win, 1, 0, "%s: %.2f%c", username1, user1Parsent, '%');
  mvwprintw(local_win, 1, col-strlen(username2)-8, "%s: %.2f%c", username2, user2Parsent, '%');

  wrefresh(local_win);
}

void totalViewersUI(WINDOW *local_win, int totalViewer){
  werase(local_win);
  wattron(local_win, COLOR_PAIR(7));
  wprintw(local_win, " LIVE ");
  wattron(local_win, COLOR_PAIR(13));
  wprintw(local_win, " 👀 %d ", totalViewer);

  wrefresh(local_win);
}

void strengthSelectUI(WINDOW *local_win, int currentStrength) {
  werase(local_win);
  wattron(local_win, COLOR_PAIR(14));
  box(local_win, 0, 0);
  curs_set(0);

  wmove(local_win, 1, 1);
  int i, maxStreng = getmaxx(local_win)-2;
  for (i = 0; i < currentStrength; i++) {
    if (i <= maxStreng/2) {
      wattron(local_win, COLOR_PAIR(3));
    } else if (i <= maxStreng/4*3) {
      wattron(local_win, COLOR_PAIR(12));
    } else if (i <= maxStreng/5*4) {
      wattron(local_win, COLOR_PAIR(15));
    } else {
      wattron(local_win, COLOR_PAIR(7));
    }
    wprintw(local_win, " ");
  }
  wrefresh(local_win);
}

void HPPlayerUI(WINDOW *HP_player1_win, WINDOW *HP_player2_win, int hp_player1, int hp_player2){
  werase(HP_player1_win);
  werase(HP_player2_win);
  int hp_width = getmaxx(HP_player1_win) ;
  int hp_player1_width = (float)hp_player1 / 1000 * hp_width;
  int hp_player2_width = (float)hp_player2 / 1000 * hp_width;

  int i;

  wmove(HP_player1_win, 0, 0);
  wattron(HP_player1_win, COLOR_PAIR(15));
  for(i = 0; i < hp_player1_width-1; i++){
    wprintw(HP_player1_win, " ");
  }
  // wattron(HP_player1_win, COLOR_PAIR(15));
  // for(int i = hp_player1_width; i < hp_width; i++){
  //   wprintw(HP_player1_win, " ");
  // }
  
  // wmove(HP_player2_win, 0, 0);
  // wattron(HP_player2_win, COLOR_PAIR(15));
  // for(int i = 0; i < hp_width-hp_player2_width-1; i++){
  //   wprintw(HP_player2_win, " ");
  // }
  wmove(HP_player2_win, 0, hp_width-hp_player2_width);
  wattron(HP_player2_win, COLOR_PAIR(15));
  for(i = 0; i < hp_player2_width-1; i++){
    wprintw(HP_player2_win, " ");
  }
  
  wrefresh(HP_player1_win);
  wrefresh(HP_player2_win);
}

void serverCountUI(WINDOW *local_win, int count) {
  werase(local_win);
  wmove(local_win, 0, 0);

  switch(count) {
    case 3: {
      wprintw(local_win, "██████\n     █\n██████\n     █\n██████");
      break;
    }
    case 2: {
      wprintw(local_win, "██████\n     █\n██████\n█\n██████");
      break;
    }
    case 1: {
      wprintw(local_win, "   ██\n    █\n    █\n    █\n    █");
      break;
    }
  }
}

char *readFile(char *filename)
{
    FILE *fp;
    fp = fopen(filename, "r+");

    if (fp == NULL)
    {
        printf("\nCan't open file!\n");
        exit(1);
    }

    char *str = malloc(10000 * sizeof(char));
    int index = 0;
    while (1)
    {
        char c = getc(fp);
        if (c == EOF)
        {
            str[index] = '\0';
            break;
        }
        str[index++] = c;
    }

    str = realloc(str, index);
    fclose(fp);
    return str;
}

void draw_character(WINDOW *local_win, int y, int x, char *str, int color_pair)
{
    int len = strlen(str), line = 0, index_char = 0;

    wattron(local_win, COLOR_PAIR(color_pair));
    wmove(local_win, y, x);
    for (int i = 0; i < len; i++)
    {
        if (str[i] == '\n')
        {
            wmove(local_win, y + ++line, x);
            index_char = 0;
        }
        else
        {
            if (str[i] == ' ')
            {
                wmove(local_win, y + line, x + ++index_char);
            }
            else
            {
                waddch(local_win, str[i]);
                index_char++;
            }
        }
    }
    wattroff(local_win, COLOR_PAIR(color_pair));
}

void draw_character_reverse(WINDOW *local_win, int y, int x, char *str, int color_pair)
{
    int len = strlen(str), line = 0, index_char = 0;

    wattron(local_win, COLOR_PAIR(color_pair));
    wmove(local_win, y, x);
    for (int i = 0; i < len; i++)
    {
        if (str[i] == '\n')
        {
            wmove(local_win, y + ++line, x);
            index_char = 0;
        }
        else
        {
            wmove(local_win, y + line, x - ++index_char);
            if (str[i] == ' ')
            {
            }
            else
            {
                waddch(local_win, str[i]);
                // index_char++;
            }
        }
    }
    wattroff(local_win, COLOR_PAIR(color_pair));
}

void characterWinUI(
  WINDOW *fight_win, char **character_1, char **character_2,
  int *count, int char1_length, int char2_length)
{
  werase(fight_win);

  draw_character(fight_win, 0, 22, character_1[(*count) % char1_length], 9);
  draw_character_reverse(fight_win, 0, 78, character_2[(*count) % char2_length], 1);
  (*count)++;
  
  wrefresh(fight_win);
}

void allowAttackNotify(WINDOW *local_win, char *notify) {
  werase(local_win);
  wattron(local_win, COLOR_PAIR(4));
  int width = getmaxx(local_win);
  mvwprintw(local_win, 1, (width-strlen(notify))/2, "%s", notify);
  wrefresh(local_win);
}