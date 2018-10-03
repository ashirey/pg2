CC = gcc

all: myftpd

myftpd: server.o
	$(CC) -Wall -o myftpd server.o

server.o: server.c
	$(CC) -Wall -c server.c -o server.o

clean:
	-rm *.o myftpd
