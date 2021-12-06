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
#include "logic.h"
#include "linkedList.h"

#define BUFF_SIZE 255
// init
User head;
char fileName[] = "account.txt";
int confds[];
int confdTotal = 0;
// end of setup


struct ThreadArgs {
	int confd;
};

void *ThreadMain(void *threadArgs) {
	char buff[BUFF_SIZE];
	int confd;

	pthread_detach(pthread_self());

	confd = ((struct ThreadArgs *)threadArgs)->confd;
	free(threadArgs);

	while (1) {
		buff[0] = '\0';
		if (recv(confd, buff, BUFF_SIZE, 0) < 0) {
			close(confd);
			continue;
		}

		// start coding from here
		
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