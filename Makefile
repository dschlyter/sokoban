BINDIR	= bin/
CC		= g++
CFLAGS	= -w -Wall -O2
LIBS	= -pthread

all: solver

solver: solver.o state.o map.o main.o workerthread.o thread.o bucketlist.h balancedtree.h 
	$(CC) -o ${BINDIR}iss ${LIBS} solver.o state.o map.o main.o workerthread.o thread.o

debug: *.cpp
	$(CC) -o ${BINDIR}debug ${CFLAGS} -g ${LIBS} *.cpp

workerthread.o: workerthread.cpp solver.h thread.h
	$(CC) ${CFLAGS} ${LIBS} -c workerthread.cpp

thread.o: thread.cpp thread.h
	$(CC) ${CFLAGS} ${LIBS} -c thread.cpp

main.o: main.cpp solver.h
	$(CC) ${CFLAGS} ${LIBS} -c main.cpp
	
solver.o: solver.cpp solver.h map.h state.h balancedtree.h bucketlist.h
	$(CC) ${CFLAGS} ${LIBS} -c solver.cpp

state.o: state.cpp state.h
	$(CC) ${CFLAGS} ${LIBS} -c state.cpp

map.o: map.cpp map.h state.h
	$(CC) ${CFLAGS} ${LIBS} -c map.cpp

clean:
	rm -f *.o
