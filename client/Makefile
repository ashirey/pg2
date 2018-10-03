CC = gcc

all: myftp

myftp: client.o
	$(CC) -Wall -o myftp client.o

client.o: client.c
	$(CC) -Wall -c client.c -o client.o

clean:
	-rm *.o myftp
