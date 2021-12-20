#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include <string.h>
#include <locale.h>
#include <signal.h>
#include <pthread.h>

#include "scrState.h"
#include "customWindow.h"
#include "logic.h"
#include "handle.h"

#define BUFF_SIZE 1000
char buff[BUFF_SIZE];
int sockfd;

WINDOW *battle_win = NULL, *message_win = NULL, *messages_win = NULL, *messages_content_win = NULL;

int height, width;
char username[20]="", password[20]="", notify[20]="", rankString[1000]="";
int lock = 1; // control block change in main function / changed by socket thread
int fighting = 1;
int hit = 0;
int battleView = 0;
int result = 0;

void *battleThread() {
  // pthread_detach(pthread_self());
  // WINDOW *battle_win = create_newWin(height-2, width/3-2, 1, 1, 0);
  // int step = 0;
  // while(fighting) {
  //   draw_warrior_left(battle_win, step, hit);
  //   draw_warrior_right(battle_win, 1-step, hit);
  //   napms(500);
  // }
  // destroy_win(battle_win);
  return NULL;
}

void *socketThread() {
  pthread_detach(pthread_self());
  while(1) {
    memset(buff,0,strlen(buff));
    if (recv(sockfd, buff, BUFF_SIZE, 0) <= 0) {
      close(sockfd);
      endwin();
      exit(0);
    };
    buff[strlen(buff)] = '\0';
    
    
    int totalToken;
    char **data = words(buff, &totalToken, "|");
    SignalState SIGNAL = data[totalToken-1][0] - '0';
    
    memset(notify,0,strlen(notify));

    if (SIGNAL == SUCCESS_SIGNAL) {
      lock = 0;
    } else if (SIGNAL == FAILED_SIGNAL) {
      strcpy(notify, data[0]);
    } else if (SIGNAL == RESULT_SIGNAL) {
      strcpy(notify, data[0]);
      result = 1;
    } else if (SIGNAL == YELL_SIGNAL) {
      curs_set(0);
      if (messages_content_win == NULL) {
        int row, col;
        getmaxyx(messages_win, row, col);
        messages_content_win = derwin(messages_win, row-2, col-2, 1, 1);
        scrollok(messages_content_win, TRUE);
      }
      if (strcmp(data[0], username) == 0) {
        wattron(messages_content_win, COLOR_PAIR(9) | A_BOLD);
        wprintw(messages_content_win, "%s:\n", "You");
        wattroff(messages_content_win, COLOR_PAIR(9) | A_BOLD);
      } else {
        wattron(messages_content_win, COLOR_PAIR(10) | A_BOLD);
        wprintw(messages_content_win, "%s:\n", data[0]);
        wattroff(messages_content_win, COLOR_PAIR(10) | A_BOLD);
      }
      wattron(messages_content_win, COLOR_PAIR(4));
      wprintw(messages_content_win, "%s\n", data[1]);
      wrefresh(messages_content_win);
      
      if (message_win != NULL) {
        int y, x;
        curs_set(1);
        // getyx(message_win, y, x);
        
        wrefresh(message_win);
      }
    }
  }
  return NULL;
}

int waitServer(int second) {
  // this function will loop in second s for wait server answer
  int time = 0; // time for wating
  while(1) {
    napms(200); // 200ms for 1 frame
    if (!lock) {
      // block for wait data from server
      break;
    }
    if (time > second*1000) {
      // befor wait 2s, but lock is true then unblock
      break;
    }
    time += 200;
  }
  // if success
  if (lock == 0) {
    lock = 1;
    return 1;
  }
  // failed
  return 0;
}

int main(int argc, char *argv[]) {
  // socket setup
  if(argc != 3 || !isIpV4(argv[1]) || !isNumber(argv[2])) {
    printf("INVALID ERROR.!!!");
    return 0;
  }

  int SERV_PORT = atoi(argv[2]);
  char *SERV_ADDR = argv[1];

  struct sockaddr_in servaddr;

  // create a socket for client
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Error");
    exit(2);
  }

  // creation of the remote server socket informarion structure
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR);
  // connect the client to the server socket
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("Problem in connecting to the server");
    return 0;
  }
  // setup socket done
  char message[50]=""; // store input message from keyboad of user
  int choose_menu; // store LOGIN/REGISTER for display screen
  setlocale(LC_ALL, "en_US.UTF-8");
  initscr();
  raw();
  refresh();
  
  keypad(stdscr, TRUE);
  noecho();
  start_color();

  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_YELLOW);
  init_pair(3, COLOR_BLACK, COLOR_WHITE);
  init_pair(4, COLOR_WHITE, COLOR_BLACK);
  init_pair(5, COLOR_RED, COLOR_BLACK);
  init_pair(6, COLOR_CYAN, COLOR_BLACK);
  init_pair(7, COLOR_WHITE, COLOR_RED);
  init_pair(8, COLOR_BLACK, COLOR_BLACK);
  init_pair(9, COLOR_GREEN, COLOR_BLACK);
  init_pair(10, COLOR_BLUE, COLOR_BLACK);

  getmaxyx(stdscr, height, width);
  // if (height < 30 || width < 140) {
  //   printf("Open terminal with full screen!!!\n");
  //   close(sockfd);
  //   endwin();
  // }

  // this window only set background for full screen
  WINDOW *my_win = create_newWin(height, width, 0, 0, 1);

  WINDOW *auth_win = create_newWin(height-2, width/3, 1, width/3, 0);
  // blocked for choose a auth mode (login register) return by choose_menu
  while(1) {
    wattron(my_win, COLOR_PAIR(1));
    box(my_win, 0, 0);
    wrefresh(my_win);
    // screen of authenticate
    memset(username,0,strlen(username));
    memset(password,0,strlen(password));
    choose_menu = menu_window(auth_win, notify);
    werase(auth_win);
    wrefresh(auth_win);
    // clear auth_win to re-use for login_win or register_win
    if (choose_menu == LOGIN) {
      // username and password is input then store the value when input ran
      if(inputLogin(auth_win, username, password) == -1) {
        // -1:back, 0: login, 1: register (LOGIN/REGISTER in srcState)
        werase(auth_win);
        continue;
      };
    } else if (choose_menu == REGISTER) {
      // like the inputLogin
      if (inputRegister(auth_win, username, password) == -1) {
        werase(auth_win);
        continue;
      };
    } else {
      // quit
      break;
    }
    werase(auth_win);
    wrefresh(auth_win);

    // being able to come here means username and password is fielded
    // printw("%s", password);
    memset(buff,0,strlen(buff));
    strcat(buff, username);
    strcat(buff, "|");
    strcat(buff, password);
    addToken(buff, choose_menu);
    // send username and password to server
    send(sockfd, buff, strlen(buff), 0);
    // setup thread for socket listen signal by server
    pthread_t threadID;
    if (pthread_create(&threadID, NULL, socketThread, NULL) != 0) {
      close(sockfd);
    };

    // wait server respone in 2s
    if (waitServer(2) == 0) {
      // when fail, server respone a signal and a message
      continue;
    }
    // authentioncate is successfully
    werase(auth_win);
    wrefresh(auth_win);
    // change to screen that choose fight or rank
    WINDOW *port_win = create_newWin(15, width/4, 3*height/8, 3*width/8, 0);
    ScreenState choose_port;
    while(1) {
      werase(messages_win);
      werase(message_win);
      werase(battle_win);

      wattron(my_win, COLOR_PAIR(1));
      box(my_win, 0, 0);
      wrefresh(my_win);

      // if user select RANK then they are can goback
      choose_port = port_window(port_win, notify);
      /* send request to client to get/post data of battle round if choose_port == meet
        or fight.
        Get info of rank if RANK
      */ 
      // there are 3 mode of menu: fight/meet/rank/logout
      werase(port_win);
      wrefresh(port_win);

      if (choose_port == RANK) {
        WINDOW *rank_win = create_newWin(25, width/3, (height-25)/2, width/3, 1);
        // send request to get data rank
        char rankReq[20] = "send me top server";
        addToken(rankReq, GET_RANK_SIGNAL);
        // send(sockfd, rankReq, strlen(rankReq), 0);
        if (waitServer(2) == 0) {
          destroy_win(rank_win);
          continue;
        }
        // this function will block during user don't press enter
        rankUI(rank_win, rankString);
        // when press enter, program is running
        destroy_win(rank_win);
        // got back menu to choose rank/fight/meet/logout;
      } else if (choose_port == FIGHT || choose_port == MEET) {
        if (choose_port == FIGHT) {
          ScreenState mode;
          mode = select_mode_window(port_win);
          if (mode == SPEED) {
            char modeGame[20] = "select mode";
            addToken(modeGame, SPEED_SIGNAL);
            send(sockfd, modeGame, strlen(modeGame), 0);
          } else if (mode == STRENGTH) {
            char modeGame[20] = "select mode";
            addToken(modeGame, STRENGTH_SIGNAL);
            send(sockfd, modeGame, strlen(modeGame), 0);
          } else continue; // user select back

          // if (waitServer(2) == 0) continue;
          // being a player successfully
          char waitNotify[] = "Waitting for enemy";
          mvprintw(height/2, (width-strlen(waitNotify))/2, waitNotify);
          refresh();
        }
        
        // send request to server for get info of current battle
        char mess[30] = "Send me current battle";
        addToken(mess, GET_INFO_CURR_GAME);
        send(sockfd, mess, strlen(mess), 0);
        // wait
        if (choose_port == FIGHT) {
          if (waitServer(20) == 0) {
            // cancel wait
            char cancelMatch[20] = "cancel match";
            addToken(cancelMatch, CANCEL_MATCH);
            send(sockfd, cancelMatch, strlen(cancelMatch), 0);
            // 
            memset(notify,0,strlen(notify));
            strcpy(notify, "Not enemy");
            continue;
          }
        } else if (choose_port == MEET) {
          if (waitServer(2) == 0) continue;
        }

        battle_win = create_newWin(height, 3*width/4, 0, 0, 1);
        battleUI(battle_win);
        // create thread for battle fight here

        int battle_win_width = getmaxx(battle_win);
        messages_win = create_newWin(3*height/4, width-battle_win_width, 0, battle_win_width, 1);
        messagesUI(messages_win);

        if (choose_port == MEET) {
          int messages_win_height = getmaxy(messages_win);
          message_win = create_newWin(3, width-battle_win_width, messages_win_height, battle_win_width, 1);
          messageUI(message_win);
          
          memset(message,0,strlen(message));
          int c;
          curs_set(1);
          wmove(message_win, 1, 1);
          wrefresh(message_win);
          while(result == 0) {
            if (kbhit()) {
              c = getchByHLone();
              if (c == 9) {
                // press key tab for give up
                break;
              } else if (c == 127) {
                if (strlen(message) <= 0) {
                  beep();
                  continue;
                }
                message[strlen(message) - 1] = '\0';
              } else if (c == 13) {
                // key enter
                addToken(message, YELL_SIGNAL);
                send(sockfd, message, strlen(message), 0);
                memset(message,0,strlen(message));

              } else {
                if (strlen(message) >= width-battle_win_width-3) {
                  beep();
                  continue;
                }
                message[strlen(message)] = c;
                message[strlen(message)] = '\0';
              }

              eraseInline(message_win, 1, 1, width-battle_win_width-2);
              wprintw(message_win, "%s", message);
              wrefresh(message_win);
            }
          }

          // go default setup
          curs_set(0);
          result = 0;
          // goto screen that show rank/fight/meet/logout
        } else if (choose_port == FIGHT) {
          int c;
          while(result == 0) {
            if (kbhit()) {
              c = getchByHLone();
              if (c == 9) {
                // press key tab for give up
                break;
              }
            }
            napms(300);
          }

          if (result == 0) {
            char givein[20] = "give in";
            addToken(givein, GIVE_IN);
            send(sockfd, givein, strlen(givein), 0);
          }
          result = 0;
        }

      } else if (choose_port == LOGOUT) {
        // kill old socket thread and destroy wins
        pthread_cancel(threadID);
        // send request logout to server
        char str[10] = "quit";
        addToken(str, LOGOUT_SIGNAL);
        send(sockfd, str, strlen(str), 0);
        // go back auth screen
        break;
      }
    }
  }

  destroy_win(battle_win);
  destroy_win(message_win);
  destroy_win(messages_win);
  close(sockfd);
  destroy_win(auth_win);
  destroy_win(my_win);
  endwin();
  return 0;
}