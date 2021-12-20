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

#define BUFF_SIZE 1000
// init
User head = NULL, player1 = NULL, player2 = NULL;
char fileName[] = "account.txt";
int confds[100];
int confdTotal = 0;
Mode mode = -1;
// end of setup


struct ThreadArgs {
	int confd;
};

void *ThreadMain(void *threadArgs) {
	int confd;

	pthread_detach(pthread_self());

	confd = ((struct ThreadArgs *)threadArgs)->confd;
	free(threadArgs);

	char buff[BUFF_SIZE+1];
	int recvBytes;
	while (1) {
		recvBytes = recv(confd, buff, BUFF_SIZE, 0);
		if (recvBytes <= 0) {
			printf("Client is disconnect\n");
			if (player1 != NULL && player1->online == confd && player2 != NULL) {
				winLose(head, player2, player1);
				mode = -1;
				player1 = NULL;
				player2 = NULL;
			} else if (player1 != NULL && player2 != NULL && player2->online == confd) {
				winLose(head, player1, player2);
				mode = -1;
				player1 = NULL;
				player2 = NULL;
			}

			logOut(head, confd);
			close(confd);
			fflush(stdout);
			break;
		}

		// start coding from here
		int tokenTotal;
		char **data = words(buff, &tokenTotal, "|\n");
		SignalState SIGNAL = data[tokenTotal-1][0] - '0';

		switch(SIGNAL) {
			// user feature
			case LOGIN_SIGNAL: {
				// printf("%d", tokenTotal);
				if (tokenTotal == 3) {
					// printf("%s", data[0]);
					logIn(head, confd, data[0], data[1]);
					// break;
				} else {
					// error
					printf("Error here");
				}
				break;
			}
			case REGISTER_SIGNAL: {
				if (tokenTotal == 3) {
					head = signUp(head, confd, data[0], data[1]);
				} else {
					// error
				}
				break;
			}
			case LOGOUT_SIGNAL: {
				logOut(head, confd);
				break;
			}
			// switch play or view rank 
			case GET_RANK_SIGNAL: {
				break;
			}
			case SPEED_SIGNAL: {
				if (mode == -1 && player1 == NULL && player2 == NULL) {
					mode = SPEED;
					player1 = player(head, confd);
					player2 = NULL;
				} else if (mode == SPEED && player1 != NULL && player2 == NULL) {
					player2 = player(head, confd);
				}
				break;
			}
			case STRENGTH_SIGNAL: {
				if (mode == -1 && player1 == NULL && player2 == NULL) {
					mode = STRENGTH;
					player1 = player(head, confd);
				} else if (mode == STRENGTH && player1 != NULL && player2 == NULL) {
					player2 = player(head, confd);
				}
				break;
			}
			case GET_INFO_CURR_GAME: {
				getInfoCurrGame(head, player1, player2, confd);
				break;
			}
			case CANCEL_MATCH: {
				mode = -1;
				player1 = NULL;
				player2 = NULL;
				break;
			}
			case ATTACK_SIGNAL: {
				break;
			}
			case GIVE_IN: {
				if (player1->online == confd) {
					winLose(head, player2, player1);
				} else if (player2->online == confd) {
					winLose(head, player1, player2);
				}
				mode = -1;
				player1 = NULL;
				player2 = NULL;
				break;
			}
			case YELL_SIGNAL: {
				yell(head, data[0], confd);
				break;
			}
			case BET: {
				// this case only used by viewer
				break;
			} default: {
				// error notify
			}
		}
		memset(buff,0,strlen(buff));
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
		pthread_t threadID;
		struct ThreadArgs *threadArgs;
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