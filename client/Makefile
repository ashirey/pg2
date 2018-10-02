CC = gcc

all: client

client: client.o
	$(CC) -Wall -o client client.o

client.o: client.c
	$(CC) -Wall -c client.c -o client.o

clean:
	-rm *.o
