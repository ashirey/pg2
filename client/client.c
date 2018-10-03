// tcp client

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>

#define SERVER_PORT 41044
#define MAX_LINE 4096

int main(int argc, char* argv[]){
	FILE *fp;
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	char buf[MAX_LINE];
	int s;
	int len;

	if(argc == 2){
		host = argv[1];
	}
	else{
		printf("pass in host name as argument\n");
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
	sin.sin_port = htons(SERVER_PORT);

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
		if(!strncmp(buf, "Exit", 4)){
			printf("exiting\n");
			break;
		}

		len = strlen(buf) + 1;
		if(send(s, buf, len, 0) == -1){
			perror("client send error");
			exit(1);
		}
		printf("enter a command:\n");
	}

	close(s);

	return 0;
}
