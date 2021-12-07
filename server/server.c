#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <pthread.h>

#include "handle.h"
#include "linkedList.h"
#include "logic.h"

#define BUFF_SIZE 255
// init
User head = NULL, player1 = NULL, player2 = NULL;
Mode GAME_MODE;
char fileName[] = "account.txt";
int confds[100];
int confdTotal = 0;
// end of setup


struct ThreadArgs {
	int confd;
};

void *ThreadMain(void *threadArgs) {
	int confd;

	pthread_detach(pthread_self());

	confd = ((struct ThreadArgs *)threadArgs)->confd;
	free(threadArgs);

	char *buff = (char*)malloc(sizeof(char)*BUFF_SIZE);
	while (1) {
		if (recv(confd, buff, BUFF_SIZE, 0) < 0) {
			close(confd);
			break;
		}


		// start coding from here
		char string[100];
		strcpy(string, buff);
		int tokenTotal;
		char **data = words(string, &tokenTotal, "|\n");
		SignalState SIGNAL = data[tokenTotal-1][0] - '0';
		printf("%d", SIGNAL);

		switch(SIGNAL) {
			// user feature
			case LOGIN: {
				if (tokenTotal == 3) {
					logIn(head, confd, data[0], data[1]);
				} else {
					// error
				}
				break;
			}
			case REGISTER: {
				if (tokenTotal == 3) {
					signUp(head, confd, data[0], data[1]);
				} else {
					// error
				}
				break;
			}
			case LOGOUT: {
				logOut(head, confd);
				break;
			}
			case DISCONNECT: {
				printf("hello");
				disconnect(head, player1, player2, confd);
				close(confd);
				return NULL;
			}
			// switch play or view rank 
			case GET_RANK: {
				break;
			}
			case PLAYER: {
				player(head, player1, player2, confd);
				break;
			}
			// select mode for game
			case MODE: {
				
				break;
			}
			// in game
			case ATTACK: {
				break;
			}
			case YELL: {
				// this case can used by player and viewer
				break;
			}
			case BET: {
				// this case only used by viewer
				break;
			} default: {
				// error notify
			}
		}
	}
	close(confd);
	return NULL;
}

int main(int argc, char *argv[]) {
	if (argc != 2 || !isNumber(argv[1])) {
		printf("INVALID INPUT!!!\n");
		exit(0);
	}
	// setup argument input
	int SERV_PORT = atoi(argv[1]);
	// read data from database (file)
	head = setup(fileName);
	// setup thread
	pthread_t threadID;
	struct ThreadArgs *threadArgs;
	int listenfd;

	// setup for share memory
	socklen_t clilen;

	struct sockaddr_in cliaddr, servaddr;
	// create of server socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error");
		return 0;
	};
	// Preparation of the socket address struct
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	// Bind the socket to the port in address
	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("Error");
		return 0;
	};
	// Listen for connection to the socket
	if (listen(listenfd, 5) < 0) {
		perror("Error");
		return 0;
	};

	clilen = sizeof(cliaddr);

	while (1) {
		// int sendBytes;
		// accept a connection request -> return a File Descriptor
		confds[confdTotal] = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

		printf("Received request...\n");
		/* Create separate memory for client argument */
		threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
		threadArgs->confd = confds[confdTotal];
		if (pthread_create(&threadID, NULL, ThreadMain, (void *)threadArgs) != 0) {
			close(confds[confdTotal]);
		};

		confdTotal++;
	}
	close(listenfd);
	return 0;
}