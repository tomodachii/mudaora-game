CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

client: client.o handle.o drawingFunc.o
	${CC} client.o handle.o drawingFunc.o -o client

client.o: client.c
	${CC} ${CFLAGS} client.c

handle.o: handle.c
	${CC} ${CFLAGS} handle.c

drawingFunc.o: drawingFunc.c
	${CC} ${CFLAGS} drawingFunc.c

clean:
	rm -f *.o *~