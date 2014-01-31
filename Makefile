BINDIR	= bin/
CC		= g++
CFLAGS	= -w -Wall -DVERIFY_SOLUTION -std=gnu++0x
OPT		= -O2

all: ${BINDIR}iss

${BINDIR}iss: *.cpp *.h
	$(CC) -o ${BINDIR}iss ${CFLAGS} ${OPT} *.cpp

debug: *.cpp *.h
	$(CC) -o ${BINDIR}debug ${CFLAGS} -O0 -g *.cpp
