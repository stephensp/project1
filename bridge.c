#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "bridge.h"
#include "strhelper.h"

int bridgeInit(bridge *b) {
	// First let's set up the socket
	int length, status, i;

	for(i = 0; i < b->numLans; i++) {
		status = socketInit(&(b->lans[i]));
		if(status == -1) {
			return status;
		}
	}

	b->numHosts = 0;
	
	FD_ZERO(&b->fdsoc); 

	// Add each LAN socket 
	for(i = 0; i < b->numLans; i++ ) {
		FD_SET(b->lans[1].sockfd, &(b->fdsoc));
	}

	return 0;
}

int socketInit(lan *l) {
	struct	sockaddr_un soc;
	int	status;

	memset(&soc, 0, sizeof(soc));
	l->sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	printf( "setting %s sockfd to %d\n", &(l->name[1]), l->sockfd);
	fflush(stdout);
	if(l->sockfd == -1) {
		char * error_buffer = malloc(256);
		sprintf(error_buffer,
			"Error: failed to create socket.");
		perror(error_buffer);
		fflush(stdout);
		return -1;
	}

	soc.sun_family = AF_UNIX;
	
	printf( "l->name = %d%s\n",l->name[0], &(l->name[1]));
	fflush(stdout);
	strcpy_lan_name(soc.sun_path, l->name);

	status = connect(l->sockfd, (struct sockaddr *) &soc, sizeof(soc));
	if(status == -1) {
		printf("Error: failed to connet to server\n");
		char * error_buffer = malloc(256);
		sprintf(error_buffer,
			"Error: failed to connect to server at '\\0%s' ",
			soc.sun_path + 1);
		perror(error_buffer);
		fflush(stdout);
		return -1;
	}
	printf( "Sucessfully connected to lan %s\n", &(l->name[1]));
	fflush(stdout);

	return 0;
}
int bridgeRun(bridge *b) {
	char	buf[MAXBUF];
	int	status, bytes_read;
	int	i, j;
	

//	status = waitMessage(b);
//	while(status >= 0) {	
	for(j = 0; j < 2; j++) {
		status = waitMessage(b);
		printf("returned status message is %d\n", status);
		fflush(stdout);
		for(i = 0; i < b->numLans; i++) {
			printf("Checking lan# %d\n", i);
			fflush(stdout);
			if(FD_ISSET(b->lans[i].sockfd, &b->fdsoc) != 0){
				fprintf(f , "Lan %s is ready\n", 
						&(b->lans[i].name[1]));
				bytes_read = read(b->lans[i].sockfd, buf, MAXBUF);
				fflush(stdout);
				printf( "%d bytes read from lan %s\n", bytes_read
						, &(b->lans[i].name[1]));
				printf( "Message read: %s\n", buf);

			}
		}
	}
	return 0;
}

int waitMessage(bridge *b) {
	struct	timeval	timeout;
	int	i, rs; 

	// Set time limit
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	

//	for(i = 0; i < b->numLans; i++) {
//		FD_CLR(b->lans[i].sockfd, &(b->fdsoc));
//	}
	FD_ZERO(&b->fdsoc); 

	// Add each LAN socket 
	for(i = 0; i < b->numLans; i++ ) {
		FD_SET(b->lans[1].sockfd, &(b->fdsoc));
	}

	rs = select(sizeof(b->fdsoc)*MAXFD, &b->fdsoc, NULL, NULL, 
			&timeout);

	printf("returnig from waitMessage\n");
	printf( "rs is?? %d\n", rs);
	fflush(stdout);
	if(rs == -1) {
		printf( "Error: Select error\n");
		perror("Select error");
		fflush(stdout);
		return -1;
	}
	if(rs == 0) {
		printf( "Select timed out\n");
		fflush(stdout);
		return -2;
	}

	return 0;

}

lan* findHost(short hostName) {
	int	length, i;

	length = sizeof(host_list)/sizeof(host);
	printf( "sizeof host_list is %d\n", length);
	fflush(stdout);
	for(i = 0; i < length; i++) {
		if(host_list[i].name == hostName) {
			return &(host_list[i].lanOn);
		}
	}

	// This means the host isn't in our current list
	return NULL;
}

int addHost(bridge *b, int lanNum, int hostName) {
	host_list = (host *)realloc(host_list, sizeof(host_list) + sizeof(host));
	b->numHosts++;
	
	host_list[b->numHosts-1].lanOn = b->lans[lanNum];
	host_list[b->numHosts-1].name = hostName;

	
	return 0;
}
int writeToAllLans(bridge *b, char *buf, int bytes_read) {
	int 	bytes_written;
	int 	i;

	
	for(i = 0; i < b->numLans; i++) {
		
		printf("bytes_read = %d, bytes_written = %d\n", bytes_read, bytes_written);
		fflush(stdout);
		bytes_written = write(b->lans[i].sockfd, buf, bytes_read);
		printf( "Writing %s to %s \n", buf, &(b->lans[i].name[1]));
		fflush(stdout);
		if(bytes_written != bytes_read) {
			if (bytes_written > 0) {
				printf( "Error: Partial write");
				fprintf(stderr, "Error: Partial write");
				fflush(stdout);
			} else {
				printf( "Error: No write");
				perror("Write error");
				fflush(stdout);
			}
			return -1;
		}
	}
	return 0;
}

int bridgeClose(bridge * b) {
	int 	i;
	
	for(i = 0; i < b->numLans; i++) {
		close(b->lans[i].sockfd);
	}
	return 0;
}
