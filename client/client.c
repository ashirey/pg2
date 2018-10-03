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
int list(int s);

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
	printf("enter a command:\t");
	while (fgets(buf, sizeof(buf), stdin)){
		buf[MAX_LINE-1] = '\0';
		if(!strncmp(buf, "EXIT", 4)){
			printf("exiting\n");
			break;
		}
		// DL
		else if(!strncmp(buf, "DL", 2)){
			download(buf, s);
		}
		else if(!strncmp(buf, "LS", 2)){
			list(s);
		}
		printf("enter a command:\t");
	}

	close(s);

	return 0;
}

int list(int s){
	// send ls command to server
	char * cmd = "LS";
	char buf[MAX_LINE];
	ssize_t len;

	if(send(s, cmd, sizeof(cmd), 0)==-1){
		perror("client send error");
		return -1;
	}
	// receive size of directory listing, and go into loop
	if((len=recv(s, buf, sizeof(buf), 0))==-1){
		perror("received errorUMM\n");
		return -1;
	}
	printf(buf);

	return 0;

	// display listings to user
}

int download(char buf[MAX_LINE], int s){
	// send command
	char * cmd = "DL";
	if(send(s, cmd, sizeof(cmd), 0) == -1){
		perror("client send error");
		exit(1);
	}

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

		char buf[MAX_LINE];
		int len;

		// get 32 bit int indicating if file exists
		int32_t int32;
		char int_str[MAX_LINE];
		if((len=recv(s, &int32, sizeof(int32_t), 0))==-1){
			perror("received errorUMM");
			exit(1);
		}
		//ntohl(
		printf("% int\n", int32);

		// get server md5 hash
		if((len=recv(s, buf, sizeof(buf), 0))==-1){
			perror("received error");
			exit(1);
		}

		// compare hashes
		FILE * fp;
		char md5cmd[MAX_LINE];
		sprintf(md5cmd, "md5sum %s", f);
		if((fp=popen(md5cmd, "r")) == NULL){
			perror("md5 sum failed");
			exit(1);
		}
		char * hash;
		char hash_buf[MAX_LINE];
		while(fgets(hash_buf, sizeof(hash_buf), fp) != NULL){
			hash = strtok(hash_buf, " ");
			printf("%s\n", hash);
		}
		strcpy(hash_buf, hash);
		printf("hashes: %s %s\n", buf, hash_buf);
		// get file size or -1

		return 0;
	}
	else{
		printf("no file name given\n");
		return 1;
	}
}
