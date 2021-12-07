CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: project

project: bigProject.o drawingFunc.o handle.o
	${CC} bigProject.o drawingFunc.o handle.o -o project

bigProject.o: bigProject.c
	${CC} ${CFLAGS} bigProject.c

drawingFunc.o: drawingFunc.c
	${CC} ${CFLAGS} drawingFunc.c

handle.o: handle.c
	${CC} ${CFLAGS} handle.c
	
clean:
	rm -f *.o *~