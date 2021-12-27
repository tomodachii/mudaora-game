CFLAGS = -c -Wall 
CC = gcc
LIBS =  -lm 

client: client.o customWindow.o handle.o
	${CC} -pthread client.o customWindow.o handle.o -o client -lncursesw

client.o: client.c
	${CC} ${CFLAGS} client.c

handle.o: handle.c
	${CC} ${CFLAGS} handle.c

customWindow.o: customWindow.c
	${CC} ${CFLAGS} customWindow.c

clean:
	rm -f *.o *~