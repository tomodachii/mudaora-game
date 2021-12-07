#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

#include "logic.h"
#include "handle.h"

#include "drawingFunc.h"
#include "const.h"

#define BUFF_SIZE 255

int sockfd;
char sendline[BUFF_SIZE + 1], recvline[BUFF_SIZE + 1];

void sighandler(int signalType)
{
  char str[20] = "quit";
  addToken(str, DISCONNECT_SIGNAL);
  if (send(sockfd, str, strlen(str), 0) < 0)
  {
    perror("Error");
  };
  close(sockfd);
  exit(1);
}

void *listenThread(void *arg) {
  while(state != QUIT) {
    recv(sockfd, recvline, BUFF_SIZE, 0);
    int tokenTotal;
    char **data = words(recvline, &tokenTotal, "|");
    state = data[tokenTotal-1] - '0';
  }
}

int main(int argc, char *argv[])
{

  if (argc != 3 || !isIpV4(argv[1]) || !isNumber(argv[2]))
  {
    printf("INVALID ERROR.!!!");
    return 0;
  }

  int SERV_PORT = atoi(argv[2]);
  char *SERV_ADDR = argv[1];

  struct sockaddr_in servaddr;

  // create a socket for client
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Error");
    exit(2);
  }

  // creation of the remote server socket informarion structure
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR);
  // connect the client to the server socket
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("Problem in connecting to the server");
    return 0;
  }

  /* Signal handler specification structure */
  signal(SIGINT, (void (*)(int))sighandler);
  // write code from here

  // send(sockfd, (void *)data, strlen(data), 0);
  // char buffer[BUFF_SIZE];
  // recv(sockfd, buffer, BUFF_SIZE, 0);
  // printf("");

  state = 0; // state = MENU;
  drawMainMenu();
  while (1)
  {
    if (state == MAIN_MENU)
    {
      drawMainMenu();
    }
    if (state == LOGIN)
    {
      drawLoginPage(sockfd);  
    }
    if (state == SIGN_UP)
    {
      drawSignUpPage(sockfd);
    }
    if (state == QUIT)
    {
      quit();
      break;
    }
  }
  // printf("Doi hu ao dua em vao con me\n");
  // delay(10000);
  // printf("Doi hu ao dua em vao con me\n");
  // delay(10000);
  // printf("Doi hu ao dua em vao con me\n");
  // delay(10000);
  // printf("Doi hu ao dua em vao con me\n");
  // delay(10000);
  sighandler(1);
  return 0;
}