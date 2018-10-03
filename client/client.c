// tcp client

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>

#define MAX_LINE 256

int download(char[], int s);
int list(char[], ins s);

int main(int argc, char* argv[]){
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	char buf[MAX_LINE];
	int s;
	int len;
	int port;

	if(argc == 3){
		host = argv[1];
		port = atoi(argv[2]);
	}
	else{
		printf("pass in host name and port as argument\n");
		exit(1);
	}

	hp = gethostbyname(host);
	if(!hp){
		fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
		exit(1);
	}

	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(port);

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		perror("simplex-talk: socket");
		exit(1);
	}
	printf("client started\n");

	if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("simplex-talk: connect");
		close(s);
		exit(1);
	}
	printf("client connected\n");

	// prompt for input
	printf("enter a command:\n");
	while (fgets(buf, sizeof(buf), stdin)){
		buf[MAX_LINE-1] = '\0';
		if(!strncmp(buf, "EXIT", 4)){
			printf("exiting\n");
			break;
		}
		else if(!strncmp(buf, "DL", 2)){
			download(buf, s);
		}
		else if(!strncmp(buf, "LS", 2)){
			list(buf, s);
		}
		printf("enter a command:\n");
	}

	close(s);

	return 0;
}

int list(char[], int s){
	// send ls command to server

	// receive size of directory listing, and go into loop

	// display listings to user
}


int download(char buf[MAX_LINE], int s){
	// get file name
	char *f;
	short int file_len;
	char file_buf[MAX_LINE];

	f = strtok(buf, " ");
	if(f){
		f = strtok(NULL, " \n");
		//file_len = strlen(f);
		//snprintf(file_buf, sizeof(file_buf), "%d %s", file_len, f);

		// send length of file name and name
		//file_len = strlen(file_buf) + 1;
		//printf("%s, size = %d\n", file_buf, file_len);
		if(send(s, f, sizeof(f), 0) == -1){
			perror("client send error");
			exit(1);
		}

		return 0;
	}
	else{
		printf("no file name given\n");
		return 1;
	}
}
