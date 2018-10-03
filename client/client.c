// tcp client
// by Abigail Shirey (ashirey) and Rita Shultz (rshultz)
// Computer Networks Program 2


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<sys/time.h>

#define MAX_LINE 256

void download(char[], int s);
void list(char[], int s);
void rm_file(char[], int s);

int main(int argc, char* argv[]){
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	char buf[MAX_LINE];
	int s;
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

	if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("simplex-talk: connect");
		close(s);
		exit(1);
	}
	printf("client connected to %s on %d\n", host, port);

	// prompt for input
	printf("enter a command:\n");
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
			list(buf, s);
		}
		else if(!strncmp(buf, "RM", 2)){
			rm_file(buf, s);
		}
		printf("enter a command:\n");
	}

	close(s);

	return 0;
}

void list(char buf[MAX_LINE], int s){
	// send ls command to server

	// receive size of directory listing, and go into loop

	// display listings to user
}


void download(char buf[MAX_LINE], int s){
	struct timeval start, end;
	// send command
	char * cmd = "DL";
	if(send(s, cmd, sizeof(cmd), 0) == -1){
		perror("client send error");
		exit(1);
	}

	// get file name
	char *f;
	char file_buf[MAX_LINE];

	f = strtok(buf, " ");
	if(f){
		f = strtok(NULL, " \n");
		strcpy(file_buf, f);
		if(send(s, file_buf, MAX_LINE, 0) == -1){
			perror("client send error");
			exit(1);
		}

		char buf[MAX_LINE];
		int len;

		// get 32 bit int indicating if file exists
		uint32_t int32;
		if((len=recv(s, &int32, sizeof(int32_t), 0))==-1){
			perror("received errorUMM");
			exit(1);
		}
		int32 = ntohl(int32);
		if(int32 == -1){
			printf("file does not exist on server\n");
			exit(1);
		}

		// get server md5 hash
		if((len=recv(s, buf, sizeof(buf), 0))==-1){
			perror("received error");
			exit(1);
		}

		// download file actually

		FILE *fp;
		fp = fopen(f, "w");
		bzero(file_buf, MAX_LINE);
		gettimeofday(&start, 0);
		while( (len=recv(s, file_buf, sizeof(file_buf), 0)) > 0){
			if(!strcmp(file_buf, "stop")){
				break;
			}
			int write = fwrite(file_buf, sizeof(char), len, fp);
			if(write < len){
				perror("file write error");
			}
			bzero(file_buf, MAX_LINE);
			if(len == 0 || len != MAX_LINE){
				break;
			}
		}
		gettimeofday(&end, 0);
		long msec = (end.tv_sec - start.tv_sec)*1000000L + end.tv_usec - start.tv_usec;	
		double sec = 1.0*msec / 1000000L;
		fclose(fp);
		printf("%lu bytes transferred in %lf seconds\n", (unsigned long) int32, sec);

		// compare hashes
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
		}
		strcpy(hash_buf, hash);
		printf("server hash: %s \nclient hash: %s\n", buf, hash_buf);
		if(strcmp(buf, hash_buf)){
			printf("error: hashes do not match\n");
		}
		else{
			printf("success: hashes match\n");
		}
	}
	else{
		printf("no file name given\n");
	}
}

void rm_file(char buf[MAX_LINE], int s){
	// send command
	char * cmd = "RM";
	if(send(s, cmd, sizeof(cmd), 0) == -1){
		perror("client send error");
		exit(1);
	}

	// get file name
	char *f;
	char file_buf[MAX_LINE];
	int len; 
	f = strtok(buf, " ");
	if(f){
		f = strtok(NULL, " \n");
		strcpy(file_buf, f);
		bzero(buf, MAX_LINE);
		
		// send file name so server can verify exists
		if(send(s, file_buf, MAX_LINE, 0) == -1){
			perror("client send error");
			exit(1);
		}
		
		// receive positive or negative confirmation
		if((len=recv(s, buf, MAX_LINE, 0))==-1){	
			perror("received error");
			exit(1);
		}
		int result = atoi(buf);
		
		bzero(buf, MAX_LINE);
		if(result < 0){
			printf("The file does not exist on the server\n");
			return;
		}
		else{
			// check before deleting
			bzero(buf, MAX_LINE);
			printf("are you sure you want to delete %s?\n", f);
			fgets(buf, sizeof(buf), stdin);	
			if(!strncmp(buf, "Yes", 3) || !strncmp(buf, "yes", 3)){
				// send deletion request
				char * del = "delete";
				bzero(buf, MAX_LINE);
				strcpy(buf, del);
				
				if(send(s, buf, MAX_LINE, 0) == -1){
					perror("client send error");
					exit(1);
				}
				
				bzero(buf, MAX_LINE);
				// receive positive or negative confirmation of deletion
				if((len=recv(s, buf, MAX_LINE, 0))==-1){
					perror("received error");
					exit(1);
				}
				
				result = atoi(buf);
				
				if(result > 0){
					printf("file deleted\n");
				}
			}
			else{
				printf("delete abandoned by the user\n");

				// send fake deletion request
				char * del = "no delete";
				bzero(buf, MAX_LINE);
				strcpy(buf, del);
				
				if(send(s, buf, MAX_LINE, 0) == -1){
					perror("client send error");
					exit(1);
				}
				
				bzero(buf, MAX_LINE);

			}
		}
	}
	else{
		printf("error: no file name given");
	}

}
