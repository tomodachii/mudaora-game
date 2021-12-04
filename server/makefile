CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

server: server.o handle.o
	${CC} -pthread server.o handle.o -o server

server.o: server.c
	${CC} ${CFLAGS} server.c

handle.o: handle.c
	${CC} ${CFLAGS} handle.c

clean:
	rm -f *.o *~