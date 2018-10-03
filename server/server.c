// tcp server
// by Abigail Shirey (ashirey) and Rita Shultz (rshult)
// Computer Networks Program 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>
#define MAX_PENDING 5
#define MAX_LINE 256

/* Function Definitions */
int list(char buf[MAX_LINE], int s);
void download(int);

int main(int argc, char * argv[]){

	struct sockaddr_in sin, client_addr;
	char buf[MAX_LINE];
	int len, port;
	socklen_t addr_len;
	int s, new_s;
	int opt = 1;

	if(argc == 2){
		port = atoi(argv[1]);
	}
	else{
		printf("pass in host port as argument\n");
		exit(1);
	}

	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);

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

	printf("Server started, waiting\n");

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

			printf("%s\n", buf);
			// Parse LS client input
			if(!strncmp(buf, "LS", 2)){
				printf("LS Command received\n");
				int n = list(buf, s);
				if( n==-1 ){
					perror("LS failure\n");
					exit(1);
				}
			}

			if(!strncmp(buf, "DL", 2)){
				download(new_s);
			}
		}
		printf("Client closed the connection\n");
		close(new_s);
	}
	close (s);
	return 0;
}

/* Function Definitions */

int list(char buf[MAX_LINE], int s){
	int len;
	printf("The list function has been called\n");

	FILE *fp = popen("ls", "r");
	while (fgets(buf, strlen(buf), fp)){
		buf[MAX_LINE-1] = '\0';
	}
	len = strlen(buf) + 1;
	if(send(s,buf,len,0)==-1){
		perror("server send error - LS\n");
		return -1;
	}
	pclose(fp);
	return 0;
}

void download(int new_s){
	char file[MAX_LINE];
	char buf[MAX_LINE];
	FILE *f;
	int len;

	// receive file name
	if((len=recv(new_s, file, MAX_LINE, 0))==-1){
		perror("Server Received Error!");
		exit(1);
	}
	if (len==0){
		exit(1);
	}

	// check if file exists
	f = fopen(file, "r");
	int32_t int32;
	if (f == NULL){
		int32 = -1;
		// return a negative 1 to client and wait for another message
		if (send(new_s, (char*)&int32, sizeof(int32), 0) == -1){
			perror("error sending to client\n");
			exit(1);
		}
	}
	else{
		// server returns the size of the file to the client
		struct stat st;
		if (stat(file, &st) == 0){
			int32 = st.st_size;
			int32 = htonl(int32);
		}
		else{
			perror("can't get file stats\n");
		}
		// return a negative 1 to client and wait for another message
		if (send(new_s, &int32, sizeof(int32_t), 0) == -1){
			perror("error sending to client\n");
			exit(1);
		}
	
		// calculate md5 hash
		FILE * fp;
		char md5cmd[MAX_LINE];
		char output[MAX_LINE];
		char * hash;
		
		sprintf(md5cmd, "md5sum %s", file);
		if((fp=popen(md5cmd, "r")) == NULL){
			perror("md5 sum failed");
			exit(1);
		}
		while(fgets(output, sizeof(output), fp) != NULL){
			hash = strtok(output, " ");
		}
		strcpy(output, hash);
		// return md5 hash to client
		if (send(new_s, output, sizeof(output), 0) == -1){
			perror("error sending to client\n");
			exit(1);
		}
		
		// server sends file to client
		bzero(buf, MAX_LINE);
		while((len = fread(buf, sizeof(char), MAX_LINE, f))>0){
			if( send(new_s, buf, sizeof(buf), 0) < 0){
				perror("file send error\n");
				exit(1);
			}
			bzero(buf, MAX_LINE);
		
		}
		char * end_msg;
		end_msg = "stop";
		strcpy(buf, end_msg);
		if( send(new_s, buf, MAX_LINE, 0) < 0){
				perror("file send error\n");
				exit(1);
		}

	}
}
