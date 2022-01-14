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
#include <time.h>

#include "handle.h"
#include "linkedList.h"
#include "logic.h"

#define BUFF_SIZE 1000
// init
User head = NULL, player1 = NULL, player2 = NULL;
int bet1 = 0, bet2 = 0;
char fileName[] = "account.txt";
int confds[100];
int confdTotal = 0, totalViewer = 0;
int turn, receivingAttack = 0; // receivingAttack used to check for mode speed
Mode mode = -1;
int isAttack = 0; // check for who is attacker and being attacked
// end of setup

struct ThreadArgs {
	int confd;
};

pthread_t threadClockID;
// this thread will count random seconds and receive attack signal in speed mode (3s-8s)
void *clockThread() {
	pthread_detach(pthread_self());
	srand((int)time(0));
	int r = 3 + rand() % (8 + 1 - 3);
	sleep(r);
	if (player1 != NULL && player2 != NULL) {
		allowAttack(head, player1, player2);
		receivingAttack = 1; //allow
	}
	return NULL;
}

void *ThreadMain(void *threadArgs) {
	int confd, isbetted = 0, isViewer = 0;

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
				bet1 = 0;
				bet2 = 0;
				totalViewer = 0;
				turn = -1;
				receivingAttack = 0;
			} else if (player1 != NULL && player2 != NULL && player2->online == confd) {
				winLose(head, player1, player2);
				mode = -1;
				player1 = NULL;
				player2 = NULL;
				bet1 = 0;
				bet2 = 0;
				totalViewer = 0;
				turn = -1;
				receivingAttack = 0;
			} else if(isViewer == 1){
				totalViewer --;
				leave_stream(head);
			}

			pthread_cancel(threadClockID);
			logOut(head, confd);
			close(confd);
			fflush(stdout);
			break;
		}

		// start coding from here
		// printf("%s printed in line 64\n", buff);
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
				getRank(head, confd);
				break;
			}
			case SPEED_SIGNAL: {
				// printf("speed signal\n");
				if (mode == -1 && player1 == NULL && player2 == NULL) {
					mode = SPEED;
					player1 = player(head, confd);
					player2 = NULL;
					turn = player1->online;
					// printf("%s, %d is player1\n", player1->username, player1->online);
				} else if (mode == SPEED && player1 != NULL && player2 == NULL) {
					player2 = player(head, confd);
					bet1 = 1;
					bet2 = 1;
				} else {
					playerError(confd);
				}
				break;
			}
			case STRENGTH_SIGNAL: {
				if (mode == -1 && player1 == NULL && player2 == NULL) {
					mode = STRENGTH;
					player1 = player(head, confd);
					player2 = NULL;
					turn = player1->online;
				} else if (mode == STRENGTH && player1 != NULL && player2 == NULL) {
					player2 = player(head, confd);
					bet1 = 1;
					bet2 = 1;
				} else {
					playerError(confd);
				}
				break;
			}
			case GET_INFO_CURR_GAME: {
				// printf("get info by %d\n", confd);
				if (player1 != NULL && player2 != NULL) {
					if (confd != player1->online && confd != player2->online) {
						isViewer = 1;
						totalViewer++;
					}
				} else {
					isViewer = 0;
					totalViewer = 0;
				}
				getInfoCurrGame(head, player1, bet1, player2, bet2, confd, totalViewer);

				if (mode == SPEED && player1 != NULL && player2 != NULL) {
					if (pthread_create(&threadClockID, NULL, clockThread, NULL) != 0) {
						close(confd);
					};
				}
				break;
			}
			case CANCEL_MATCH: {
				// printf("%d is cancel match\n", confd);
				mode = -1;
				player1 = NULL;
				player2 = NULL;
				bet1 = 0;
				bet2 = 0;
				totalViewer = 0;
				turn = -1;
				break;
			}
			case ATTACK_SIGNAL: {
				if (mode == STRENGTH && turn == confd) {
					attack(head, player1, player2, atoi(data[0]), confd);
					if (turn == player1->online) {
						turn = player2->online;
					} else if (turn == player2->online) {
						turn = player1->online;
					}
				} else if (mode == SPEED && receivingAttack) {
					attack(head, player1, player2, 100, confd);
					receivingAttack = 0; //blocked attack

					if (pthread_create(&threadClockID, NULL, clockThread, NULL) != 0) {
						close(confd);
					};
				}

				if (player1->hp <= 0 || player2->hp <= 0) {
					mode = -1;
					player1 = NULL;
					player2 = NULL;
					bet1 = 0;
					bet2 = 0;
					totalViewer = 0;
					turn = -1;
					receivingAttack = 0;
				}
				break;
			}
			case GIVE_IN: {
				// printf("%d is give up\n", confd);
				if (player1->online == confd) {
					winLose(head, player2, player1);
				} else if (player2->online == confd) {
					winLose(head, player1, player2);
				}
				
				pthread_cancel(threadClockID);
				
				mode = -1;
				player1 = NULL;
				player2 = NULL;
				bet1 = 0;
				bet2 = 0;
				totalViewer = 0;
				turn = -1;
				receivingAttack = 0;
				break;
			}
			case YELL_SIGNAL: {
				yell(head, data[0], confd);
				break;
			}
			case BET_P1:{				
				if(isbetted == 0){
					bet1++;
					bet(head, player1, bet1, player2, bet2);
					isbetted = 1;
					
				}
				break;
			}
			case BET_P2:{				
				if(isbetted == 0){
					bet2++;
					bet(head, player1, bet1, player2, bet2);
					isbetted = 2;
					
				}
				break;
			}
			case LEAVE_STREAM:{
				totalViewer--;
				isViewer = -1;
				leave_stream(head);
				break;
			}
			default: {
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