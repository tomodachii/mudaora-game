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
int totalViewer = 0, isDrew = 0;

int height, width;
char username[20]="", password[20]="", notify[20]="", rankString[1000]="", message[50]="";
char player1[20]="",player2[20]="";
int rate1 = 0, rate2 = 0;
int hp_player1 = 1000, hp_player2 = 1000, hp_width;
int currStrength = 1;
char **str_stand, **str_ready, **str_punch, **str_stun;

int isReceivingMsg = 0;
int lock = 1; // control block change in main function / changed by socket thread
int turn = 0; // turn of attack
int result = 0;
int fightMode = -1; // -1: no mode; 0: speed mode; 1: strength mode;
int turnOfCharacter = 0; // -1: palyer1 is attacker; 0: no one is attacker; 1: player2 is attacker

pthread_t threadSelectStrengthID, characterThreadID;

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
        *messages_content_win_of_viewer, 
        *viewer_total_win = NULL, 
        *HP_player1_win = NULL,
        *HP_player2_win = NULL,        
        *select_strength_win = NULL,
        *character_win = NULL;
        
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
  // display total of viewers
  viewer_total_win = derwin(battle_win, 1, 14, 1, 1);

  // display HP of player 1
  HP_player1_win = derwin(battle_win, 1, battle_win_width/3, 3, 1);
  // display HP of player 2
  int hp_player1_width = getmaxx(HP_player1_win);
  HP_player2_win = derwin(battle_win, 1, battle_win_width/3-1, 3, battle_win_width - hp_player1_width);
  // display strength selector
  hp_width = hp_player1_width;
  int select_strength_win_width = battle_win_width-hp_player1_width*2;
  select_strength_win = derwin(battle_win, 3, select_strength_win_width, 2, (battle_win_width-select_strength_win_width)/2);

  // display 2 characters
  character_win = derwin(battle_win, 28, battle_win_width-2, 7, 1);


  str_stand = (char **)malloc(6 * sizeof(char *));
  str_stand[0] = readFile("./text/stand_00.txt");
  str_stand[1] = readFile("./text/stand_01.txt");
  str_stand[2] = readFile("./text/stand_02.txt");
  str_stand[3] = readFile("./text/stand_03.txt");
  str_stand[4] = readFile("./text/stand_02.txt");
  str_stand[5] = readFile("./text/stand_01.txt");

  str_ready = (char **)malloc(6 * sizeof(char *));
  str_ready[0] = readFile("./text/ready_00.txt");
  str_ready[1] = readFile("./text/ready_01.txt");
  str_ready[2] = readFile("./text/ready_02.txt");
  str_ready[3] = readFile("./text/ready_03.txt");

  int str_len = strlen(str_stand[0]);

  str_punch = (char **)malloc(6 * sizeof(char *));
  str_punch[0] = (char *)malloc((str_len + 1) * sizeof(char));
  strcpy(str_punch[0], str_stand[0]);
  str_punch[1] = readFile("./text/punch_01.txt");
  str_punch[2] = readFile("./text/punch_02.txt");
  str_punch[3] = readFile("./text/punch_03.txt");

  str_stun = (char **)malloc(6 * sizeof(char *));
  str_stun[0] = (char *)malloc((str_len + 1) * sizeof(char));
  str_stun[1] = (char *)malloc((str_len + 1) * sizeof(char));
  strcpy(str_stun[0], str_stand[0]);
  strcpy(str_stun[1], str_stand[0]);
  str_stun[2] = readFile("./text/stun_02.txt");
  str_stun[3] = readFile("./text/stun_03.txt");
}

void *strengthThread() {
  pthread_detach(pthread_self());
  int v = 1;
  int length = getmaxx(select_strength_win);
  while(1) {
    if (currStrength > length-2) {
      v = -1;
    } else if (currStrength <= 2) {
      v = 1;
    }

    napms(300);
    currStrength += v;
    if (currStrength > 1 && currStrength < length-2) {
      strengthSelectUI(select_strength_win, currStrength);
    }
  }
  return NULL;
}

int count = 0; // index of frame of players
void *characterThread() {
  pthread_detach(pthread_self());

  while(result == 0 && isViewer != -1) {
    
    switch(turnOfCharacter) {
      case -1: {
        isDrew = 0;
        characterWinUI(character_win, str_punch, str_stun, &count, 4, 4);
        isDrew = 1;
        if(count >= 4) {
          turnOfCharacter = 0;
          count = 0;
        }
        break;
      }
      case 0: {
        isDrew = 0;
        characterWinUI(character_win, str_stand, str_stand, &count, 5, 5);
        isDrew = 1;
        if(count >= 5) {
          count = 0;
        }
        break;
      }
      case 1: {
        isDrew = 0;
        characterWinUI(character_win, str_stun, str_punch, &count, 4, 4);
        isDrew = 1;
        if(count >= 4) {
          turnOfCharacter = 0;
          count = 0;
        }
        break;
      }
      default: {}
    }
    wrefresh(message_win);
    napms(500);
  }

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
      turn = -1;
      fightMode = -1;
    } else if (SIGNAL == GET_RANK_SIGNAL) {
      memset(rankString, 0, strlen(rankString));
      strcpy(rankString, data[0]);
      lock = 0;
    } else if (SIGNAL == BET){

      if (!isReceivingMsg || isViewer != 1) continue;
      
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
      turnOfCharacter = 0;

      int total;
      char **users = words(data[0], &total, " ");
      // username1:bet1 username2:bet2 totalViewer [G]
      if (total == 7) {
        turn = 1;
      } else if (total == 6){
        turn = 0;
      }
      char **user1 = words(users[0], &total, ":");
      char **user2 = words(users[1], &total, ":");
      totalViewer = atoi(users[2]);
      strcpy(player1, user1[0]);
      strcpy(player2, user2[0]);
      rate1 = atoi(user1[1]);
      rate2 = atoi(user2[1]);

      if (isViewer == 1) {
        hp_player1 = atoi(users[3]);
        hp_player2 = atoi(users[4]);
      }

      wattron(battle_win, A_BOLD);
      mvwprintw(battle_win, 4, 1 , "%s ", user1[0]);
      wattron(battle_win, COLOR_PAIR(16));

      if(isViewer == 1){
        wprintw(battle_win, " %d ", hp_player1);
        mvwprintw(battle_win, 4, battle_win_width - strlen(user2[0]) - strlen(users[4]) - 4 , " %d ", hp_player2);
      } else {
        wprintw(battle_win, " 1000 ");
        mvwprintw(battle_win, 4, battle_win_width - strlen(user2[0]) - strlen(users[4]) - 4 , " 1000 ");
      }
      wattroff(battle_win, COLOR_PAIR(16));
      wprintw(battle_win, " %s", user2[0]);
      wattroff(battle_win, A_BOLD);

      wrefresh(battle_win);

      HPPlayerUI(HP_player1_win, HP_player2_win, hp_player1, hp_player2);
      lock = 0;

    } else if (SIGNAL == ALLOW_ATTACK_SIGNAL) {
      turn = -1;
      if (fightMode == 0) {
        allowAttackNotify(select_strength_win, data[0]);
        beep();
      }
    }
    else if(SIGNAL == LEAVE_STREAM){
      if (isViewer == -1){
        continue;
      }
      totalViewer--;
      totalViewersUI(viewer_total_win, totalViewer);
      if (isViewer) {
        wrefresh(message_win);
      }
    }
    else if(SIGNAL == JOIN_STREAM){
      if (isViewer == -1){
        continue;
      }
      totalViewer++;
      totalViewersUI(viewer_total_win, totalViewer);
      if (isViewer) {
        wrefresh(message_win);
      }
    }
    else if(SIGNAL == ATTACK_SIGNAL){
      if (isViewer == -1) {
        continue;
      }
      int total, len_of_hp_player2;
      count = 0;
      char **users = words(data[0], &total, " ");
      char **user1 = words(users[0], &total, ":");
      char **user2 = words(users[1], &total, ":");
      if (strcmp(user1[0], username) != 0) {
        turn = 1;
      }

      if (strcmp(user1[0], player1) == 0) {
        // user1 is attacker; user2 is being attacked
        turnOfCharacter = -1;
        hp_player1 = atoi(user1[1]);
        hp_player2 = atoi(user2[1]);
        len_of_hp_player2 = strlen(user2[1]);
      } else if (strcmp(user2[0], player1) == 0) {
        turnOfCharacter = 1;
        hp_player1 = atoi(user2[1]);
        hp_player2 = atoi(user1[1]);
        len_of_hp_player2 = strlen(user1[1]);
      }

      if (fightMode == 0) {
        werase(select_strength_win);
        wrefresh(select_strength_win);
      }

      wattron(battle_win, A_BOLD);
      mvwprintw(battle_win, 4, 1 , "%s ", user1[0]);
      wattron(battle_win, COLOR_PAIR(16));
      wprintw(battle_win, " %d ", hp_player1);

      wattron(battle_win, COLOR_PAIR(1));
      mvwprintw(battle_win, 4, battle_win_width - strlen(user2[0]) -len_of_hp_player2 - 4 , "     ");
      wattron(battle_win, COLOR_PAIR(16));
      mvwprintw(battle_win, 4, battle_win_width - strlen(user2[0]) -len_of_hp_player2 - 4 , " %d ", hp_player2);
      wattroff(battle_win, COLOR_PAIR(16));
      wprintw(battle_win, " %s", user2[0]);
      wattroff(battle_win, A_BOLD);

      wrefresh(battle_win);

      HPPlayerUI(HP_player1_win, HP_player2_win, hp_player1, hp_player2);
    }
    else if (SIGNAL == YELL_SIGNAL) {
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
  init_pair(13, COLOR_WHITE, COLOR_MAGENTA);
  init_pair(14, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(15, COLOR_WHITE, COLOR_GREEN);
  init_pair(16, COLOR_RED, COLOR_BLUE);

  getmaxyx(stdscr, height, width);
  if (height < 30 || width < 140) {
    close(sockfd);
    endwin();
    printf("Open terminal with full screen!!!\n");
    return 0;
  }

  init();

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
      pthread_cancel(characterThreadID);

      isReceivingMsg = 0;
      isViewer = -1;
      turn = -1;
      result = 0;

      werase(character_win);
      werase(select_strength_win);
      werase(viewer_total_win);
      werase(battle_win);
      werase(message_win);
      werase(messages_content_win);
      werase(messages_content_win_of_viewer);
      werase(messages_content_win_of_fighter);
      werase(messages_win_of_viewer);
      werase(messages_win_of_fighter);
      werase(bet_win);
      werase(my_win);
      
      wrefresh(messages_content_win_of_fighter);
      wrefresh(messages_content_win_of_viewer);
      wrefresh(messages_win_of_viewer);
      wrefresh(messages_win_of_fighter);
      wrefresh(bet_win);
      wrefresh(messages_content_win);
      wrefresh(message_win);
      wrefresh(battle_win);
      wrefresh(viewer_total_win);
      wrefresh(select_strength_win);
      wrefresh(character_win);

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
        fightMode = -1;
        if (choose_port == FIGHT) {
          ScreenState mode;
          mode = select_mode_window(port_win);
          if (mode == SPEED) {
            memset(buff,0,strlen(buff));
            strcpy(buff, "select mode");
            addToken(buff, SPEED_SIGNAL);
            send(sockfd, buff, strlen(buff), 0);
            fightMode = 0; 
          } else if (mode == STRENGTH) {
            memset(buff,0,strlen(buff));
            strcpy(buff, "select mode");
            addToken(buff, STRENGTH_SIGNAL);
            send(sockfd, buff, strlen(buff), 0);
            fightMode = 1;
          } else continue; // user select back

          // being a player successfully
          char waitNotify[] = "Waitting for enemy";
          mvprintw(height/2, (width-strlen(waitNotify))/2, "%s", waitNotify);
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
          isViewer = 0;
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
          isViewer = 1;
          if (waitServer(2) == 0) continue;
        }

        battleUI(battle_win);
        // create thread for battle fight here
        totalViewersUI(viewer_total_win, totalViewer);
        HPPlayerUI(HP_player1_win, HP_player2_win, hp_player1, hp_player2);

        if (choose_port == FIGHT) {
          // not viewer
          messagesUI(messages_win_of_fighter);
          messageContentUI(messages_content_win_of_fighter);
        } else {
          // viewer
          messagesUI(messages_win_of_viewer);
          messageContentUI(messages_content_win_of_viewer);
        }
        // messagesUI(messages_win);

        if (choose_port == MEET) {

          ratioUI(ratio_win, player1, rate1, player2, rate2);

          betUI(bet_win, player1, player2);
          messageUI(message_win);
          
          memset(message,0,strlen(message));
          strcpy(message, "");
          
          int c;
          curs_set(1);
          wmove(message_win, 1, 1);
          wrefresh(message_win);

          pthread_create(&characterThreadID, NULL, characterThread, NULL);
          
          while(result == 0) {
            if (kbhit()) {
              c = getchByHLone();
              if (c == 9) {
                // press key tab for give up
                char msg[10] = "abc";
                addToken(msg, LEAVE_STREAM);
                send(sockfd, msg, strlen(msg), 0);
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
                strcpy(message, "");
                napms(100);
              } else if(c == '1'){ // Press F1, bet for player 1
                char str[10] = "smtb";
                addToken(str, BET_P1);
                send(sockfd, str, strlen(str), 0);
              } else if(c == '2'){ // Press F2, bet for player 2
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
            napms(100);
          }

          // go default setup
          curs_set(0);
          result = 0;
          // goto screen that show rank/fight/meet/logout
        } else if (choose_port == FIGHT) {
          int isThreadCreated = 0; //thread of strength bar
          int isThreadCharacter = 0;

          while(result == 0) {            
            
            if (kbhit()) {
              int c = getchByHLone();
              if (c == 9) {
                // press key tab for give up
                hp_player1 = 1000;
                hp_player2 = 1000;
                break;
              } else if (c == ' ') {
                if (turn && fightMode == 1) {
                  turn = 0;
                  pthread_cancel(threadSelectStrengthID);
                  werase(select_strength_win);
                  wrefresh(select_strength_win);
                  isThreadCreated = 0;

                  char data[20] = "";
                  sprintf(data, "%d", currStrength*2);
                  addToken(data, ATTACK_SIGNAL);
                  send(sockfd, data, strlen(data), 0);
                  currStrength = 0;
                } else if (fightMode == 0) {
                  char data[20] = "attack";
                  addToken(data, ATTACK_SIGNAL);
                  send(sockfd, data, strlen(data), 0);
                  werase(select_strength_win);
                  wrefresh(select_strength_win);
                }
              }
            }

            if (!isThreadCharacter) {
              pthread_create(&characterThreadID, NULL, characterThread, NULL);
              isThreadCharacter = 1;
            }

            napms(200);

            if (turn == 1 && !isThreadCreated && fightMode == 1) {
              // if this turn then show choose bar for strength
              if (pthread_create(&threadSelectStrengthID, NULL, strengthThread, NULL) != 0) {
                close(sockfd);
                endwin();
                printf("Create thread failed\n");
                exit(0);
              };
              isThreadCreated = 1;
            }

            napms(100);
          }

          pthread_cancel(threadSelectStrengthID);
          werase(select_strength_win);
          wrefresh(select_strength_win);
          isThreadCreated = 0;
          isThreadCharacter = 0;

          if (result == 0) {
            char givein[20] = "give in";
            addToken(givein, GIVE_IN);
            send(sockfd, givein, strlen(givein), 0);
            waitServer(2);
          }
          result = 0;
          hp_player1 = 1000;
          hp_player2 = 1000;
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