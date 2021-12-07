CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

server: server.o handle.o linkedList.o logic.o
	${CC} -pthread server.o handle.o linkedList.o logic.o -o server

server.o: server.c
	${CC} ${CFLAGS} server.c

handle.o: handle.c
	${CC} ${CFLAGS} handle.c

linkedList.o: linkedList.c
	${CC} ${CFLAGS} linkedList.c

logic.o: logic.c
	${CC} ${CFLAGS} logic.c

clean:
	rm -f *.o *~