BINDIR	= bin/
CC		= g++
CFLAGS	= -w -Wall -O2

all: ${BINDIR}iss

${BINDIR}iss: *.cpp *.h
	$(CC) -o ${BINDIR}iss ${CFLAGS} *.cpp

debug: *.cpp *.h
	$(CC) -o ${BINDIR}debug ${CFLAGS} -g *.cpp
