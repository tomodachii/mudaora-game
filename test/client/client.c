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
char string[BUFF_SIZE];
int sockfd, isViewer = -1;
int battle_win_width;

int height, width;
char username[20]="", password[20]="", notify[20]="", rankString[1000]="", message[50]="";
char player1[20]="",player2[20]="";
int rate1 = 0, rate2 = 0;

int isReceivingMsg = 0;
int lock = 1; // control block change in main function / changed by socket thread
// int fighting = 1;
// int hit = 0;
// int battleView = 0;
int result = 0;

WINDOW  *my_win=NULL, 
        *battle_win = NULL, 
        *message_win = NULL, 
        *messages_win = NULL, 
        *ratio_win = NULL, 
        *messages_content_win = NULL, 
        *bet_win = NULL, 
        *rank_win = NULL, 
        *auth_win = NULL, 
        *port_win=NULL, 
        *messages_win_of_fighter=NULL, 
        *messages_win_of_viewer=NULL, 
        *messages_content_win_of_fighter,
        *messages_content_win_of_viewer;

void init() {
  // god of all window
  my_win = create_newWin(height, width, 0, 0, 1); 
  // authentication account / login/signup
  auth_win = create_newWin(height-2, width/3, 1, width/3, 0);
  // choose the port that fight/view/rank/back
  port_win = create_newWin(15, width/4, 3*height/8, 3*width/8, 1);
  // window of rank
  rank_win = create_newWin(25, width/3, (height-25)/2, width/3, 1);
  // window that it's cover 2 boxer
  battle_win = newwin(height, 3*width/4, 0, 0);

  battle_win_width = getmaxx(battle_win);
  messages_win_of_fighter = newwin(height, width-battle_win_width, 0, battle_win_width);
  messages_win_of_viewer = newwin(height-9, width-battle_win_width, 0, battle_win_width);

  int content_win_width_of_fighter, content_win_height_of_fighter;
  getmaxyx(messages_win_of_fighter, content_win_height_of_fighter, content_win_width_of_fighter);
  messages_content_win_of_fighter = derwin(messages_win_of_fighter, content_win_height_of_fighter-2, content_win_width_of_fighter-2, 1, 1);

  int content_win_width_of_viewer, content_win_height_of_viewer;
  getmaxyx(messages_win_of_viewer, content_win_height_of_viewer, content_win_width_of_viewer);
  messages_content_win_of_viewer = derwin(messages_win_of_viewer, content_win_height_of_viewer-2, content_win_width_of_viewer-2, 1, 1);
  // messages_content_win = derwin(messages_win, row-2, col-2, 1, 1);
  // bet for user
  bet_win = newwin(4, width-battle_win_width, height-7, battle_win_width);
  // window that user can input text and emoji
  message_win = newwin(3, width-battle_win_width, height-3, battle_win_width);
  // window that display ratio bet for 2 player
  ratio_win = newwin(2, width-battle_win_width, height-9, battle_win_width);
}

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
    memset(string,0,strlen(string));
    if (recv(sockfd, string, BUFF_SIZE, 0) <= 0) {
      close(sockfd);
      endwin();
      exit(0);
    };
    string[strlen(string)] = '\0';
    
    // mvprintw(0, 0, "%s\n", string);
    // refresh();
    napms(1000);
    int totalToken;
    char **data = words(string, &totalToken, "|");
    SignalState SIGNAL = data[totalToken-1][0] - '0';
    
    memset(notify,0,strlen(notify));

    if (SIGNAL == SUCCESS_SIGNAL) {
      lock = 0;
    } else if (SIGNAL == FAILED_SIGNAL) {
      strcpy(notify, data[0]);
    } else if (SIGNAL == RESULT_SIGNAL) {
      strcpy(notify, data[0]);
      result = 1;
    } else if (SIGNAL == GET_RANK_SIGNAL) {
      memset(rankString, 0, strlen(rankString));
      strcpy(rankString, data[0]);
      lock = 0;
    } else if (SIGNAL == BET){
      
      int total;
      char **users = words(data[0], &total, " ");
      char **user1 = words(users[0], &total, ":");
      char **user2 = words(users[1], &total, ":");
      strcpy(player1, user1[0]);
      strcpy(player2, user2[0]);
      rate1 = atoi(user1[1]);
      rate2 = atoi(user2[1]);
      ratioUI(ratio_win, player1, rate1, player2, rate2);
      wrefresh(message_win);
    }    
    else if (SIGNAL == GET_INFO_CURR_GAME) {
      int total;
      char **users = words(data[0], &total, " ");
      char **user1 = words(users[0], &total, ":");
      char **user2 = words(users[1], &total, ":");
      strcpy(player1, user1[0]);
      strcpy(player2, user2[0]);
      rate1 = atoi(user1[1]);
      rate2 = atoi(user2[1]);
      lock = 0;
    } else if (SIGNAL == YELL_SIGNAL) {
      if (!isReceivingMsg) continue;
      
      if (isViewer == 0) {
        messages_content_win = messages_content_win_of_fighter;
      } else {
        messages_content_win = messages_content_win_of_viewer;
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
      
      if (isViewer) {
        curs_set(1);        
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

  // creation of the remote server socket information structure
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
  init_pair(11, COLOR_WHITE, COLOR_BLUE);
  init_pair(12, COLOR_WHITE, COLOR_YELLOW);

  getmaxyx(stdscr, height, width);
  if (height < 30 || width < 140) {
    close(sockfd);
    endwin();
    printf("Open terminal with full screen!!!\n");
    return 0;
  }

  init();

  // this window only set background for full screen
  

  
  // blocked for choose a auth mode (login register) return by choose_menu
  while(1) {
    isReceivingMsg = 0;
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
      endwin();
      printf("Create socket thread failed\n");
      return 0;
    };

    // wait server respone in 2s
    if (waitServer(2) == 0) {
      // when fail, server respone a signal and a message
      pthread_cancel(threadID);
      continue;
    }
    // authenticate is successfully
    werase(auth_win);
    wrefresh(auth_win);
    // change to screen that choose fight or rank
    ScreenState choose_port;
    while(1) {
      isReceivingMsg = 0;
      // if (messages_content_win != NULL) {
      //   werase(messages_content_win);
      //   wmove(messages_content_win, 0, 0);
      //   wrefresh(messages_content_win);
      // }
      werase(my_win);
      werase(messages_content_win_of_fighter);
      werase(messages_content_win_of_viewer);
      werase(messages_win_of_viewer);
      werase(messages_win_of_fighter);
      werase(bet_win);
      werase(messages_content_win);
      werase(message_win);
      werase(battle_win);
      
      wrefresh(messages_content_win_of_fighter);
      wrefresh(messages_content_win_of_viewer);
      wrefresh(messages_win_of_viewer);
      wrefresh(messages_win_of_fighter);
      wrefresh(bet_win);
      wrefresh(messages_content_win);
      wrefresh(message_win);
      wrefresh(battle_win);

      wattron(my_win, COLOR_PAIR(1));
      box(my_win, 0, 0);
      wrefresh(my_win);

      // if user select RANK then they are can go back
      choose_port = port_window(port_win, notify);
      /* send request to client to get/post data of battle round if choose_port == meet
        or fight.
        Get info of rank if RANK
      */ 
      // there are 3 mode of menu: fight/meet/rank/logout
      werase(port_win);
      wrefresh(port_win);

      if (choose_port == RANK) {
        
        // send request to get data rank
        memset(buff,0,strlen(buff));
        strcpy(buff, "send me top server");
        addToken(buff, GET_RANK_SIGNAL);
        send(sockfd, buff, strlen(buff), 0);

        mvprintw(height/2, (width-20)/2, "Waitting for server");
        refresh();

        if (waitServer(2) == 0) {
          werase(rank_win);
          continue;
        }
        // this function will block during user don't press enter
        rankUI(rank_win, rankString);
        // when press enter, program is running
        werase(rank_win);
        // got back menu to choose rank/fight/meet/logout;
      } else if (choose_port == FIGHT || choose_port == MEET) {
        isReceivingMsg = 1;
        if (choose_port == FIGHT) {
          
          ScreenState mode;
          mode = select_mode_window(port_win);
          if (mode == SPEED) {
            memset(buff,0,strlen(buff));
            strcpy(buff, "select mode");
            addToken(buff, SPEED_SIGNAL);
            send(sockfd, buff, strlen(buff), 0);
          } else if (mode == STRENGTH) {
            memset(buff,0,strlen(buff));
            strcpy(buff, "select mode");
            addToken(buff, STRENGTH_SIGNAL);
            send(sockfd, buff, strlen(buff), 0);
          } else continue; // user select back

          // being a player successfully
          char waitNotify[] = "Waitting for enemy";
          mvprintw(height/2, (width-strlen(waitNotify))/2, waitNotify);
          refresh();

          if (waitServer(2) == 0) continue;
        }
        
        // send request to server for get info of current battle
        memset(buff,0,strlen(buff));
        strcpy(buff, "Send me current battle");
        addToken(buff, GET_INFO_CURR_GAME);
        send(sockfd, buff, strlen(buff), 0);
        // beep();
        // wait
        if (choose_port == FIGHT) {
          if (waitServer(20) == 0) {
            // cancel wait
            memset(buff,0,strlen(buff));
            strcat(buff, "cancel match");
            addToken(buff, CANCEL_MATCH);
            send(sockfd, buff, strlen(buff), 0);
            // 
            memset(notify,0,strlen(notify));
            strcpy(notify, "There is no enemy");
            continue;
          }
        } else if (choose_port == MEET) {
          if (waitServer(2) == 0) continue;
        }

        battleUI(battle_win);
        // create thread for battle fight here

        if (choose_port == FIGHT) {
          // not viewer
          isViewer = 0;
          messagesUI(messages_win_of_fighter);
          messageContentUI(messages_content_win_of_fighter);
        } else {
          // viewer
          isViewer = 1;
          messagesUI(messages_win_of_viewer);
          messageContentUI(messages_content_win_of_viewer);
        }
        // messagesUI(messages_win);

        if (choose_port == MEET) {

          ratioUI(ratio_win, player1, rate1, player2, rate2);

          betUI(bet_win, player1, player2);
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
              } else if(c == 'a'){ // Press F1, bet for player 1
                char str[10] = "smtb";
                addToken(str, BET_P1);
                send(sockfd, str, strlen(str), 0);
              } else if(c == 'b'){ // Press F2, bet for player 2
                char str[10] = "smtb";
                addToken(str, BET_P2);
                send(sockfd, str, strlen(str), 0);
              } else{
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
          while(result == 0) {
            if (kbhit()) {
              int c = getchByHLone();
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
            waitServer(2);
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