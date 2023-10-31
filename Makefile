CC=gcc

all: filewriter

filewriter: filewriter.c
	$(CC) -s -o filewriter filewriter.c
