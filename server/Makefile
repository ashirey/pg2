CC = gcc

all: server

server: server.o
	$(CC) -Wall -o server server.o

server.o: server.c
	$(CC) -Wall -c server.c -o server.o

clean:
	-rm *.o
