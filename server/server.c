// tcp server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#define SERVER_PORT 41044
#define MAX_PENDING 5
#define MAX_LINE 4096

/* Function Declarations */
int list(char buf[MAX_LINE], int s);

int main(int argc, char * argv[]){

	struct sockaddr_in sin, client_addr;
	char buf[MAX_LINE];
	int len;
	socklen_t addr_len;
	int s, new_s;
	int opt = 1;

	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);

	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}

	// set socket option
	if ((setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)& opt, sizeof(int)))<0){
		perror ("simplex-talk:setscokt");
		exit(1);
	}

	if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		perror("simplex-talk: bind");
		exit(1);
	}
	if ((listen(s, MAX_PENDING))<0){
		perror("simplex-talk: listen");
		exit(1);
	}

	printf("Welcome to the first TCP Server!\n");

	/* wait for connection, then receive and print text */
	addr_len = sizeof (client_addr);
	while(1) {
		if ((new_s = accept(s, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
			perror("simplex-talk: accept");
			exit(1);
		}

		while (1){
			if((len=recv(new_s, buf, sizeof(buf), 0))==-1){
				perror("Server Received Error!");
				exit(1);
			}
			if (len==0) break;

			// Parse input
			if(!strncmp(buf, "LS", 2)){
				if(list(buf,s)==-1){
					printf("Error with LS\n");
					exit(1);
				}
			}

			printf("TCP Server Received:%s", buf);

			// Server side LS functionality
			if(!strncmp(buf, "LS", 2)){
				printf("LS Function beginning from server side\n");
			}


		}
		printf("Client finishes, close the connection!\n");
		close(new_s);
	}
	close (s);
	return 0;
}

/* Function Definitions */

int list(char buf[MAX_LINE], int s){
	int len;

	FILE *fp = popen("ls", "r");
	while (fgets(buf, strlen(buf), fp)){
		buf[MAX_LINE-1] = '\0';
	}
	len = strlen(buf) + 1;
	if(send(s,buf,len,0)==-1){
		perror("server send error - LS");
		return -1;
	}
	pclose(fp);
	return 0;
}
