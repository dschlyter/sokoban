BINDIR	= bin/
CC		= g++
CFLAGS	= -Wall -O2

all: solver

solver: solver.o state.o map.o client.o
	$(CC) -o ${BINDIR}solver solver.o state.o map.o client.o

client.o: client.cpp solver.h
	$(CC) ${CFLAGS} -c client.cpp

solver.o: solver.cpp solver.h map.h state.h
	$(CC) ${CFLAGS} -c solver.cpp

state.o: state.cpp state.h
	$(CC) ${CFLAGS} -c state.cpp

map.o: map.cpp map.h state.h
	$(CC) ${CFLAGS} -c map.cpp

clean:
	rm -f *.o
