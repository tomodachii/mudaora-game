CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

client: client.o handle.o
	${CC} client.o handle.o -o client

client.o: client.c
	${CC} ${CFLAGS} client.c

handle.o: handle.c
	${CC} ${CFLAGS} handle.c

clean:
	rm -f *.o *~