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
	
	return 0;
}

int socketInit(lan *l) {
	struct	sockaddr_un soc;
	int	status;

	l->sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if(l->sockfd == -1) {
		char * error_buffer = malloc(256);
		sprintf(error_buffer,
			"Error: failed to create socket.");
		perror(error_buffer);
		return -1;
	}

	soc.sun_family = AF_UNIX;
	strcpy_lan_name(soc.sun_path, l->name);
	fprintf(f,"soc.sun_path: 0%s\n", soc.sun_path);

	status = connect(l->sockfd, (struct sockaddr *) &soc, sizeof(soc));
	if(status == -1) {
		fprintf(f,"Error: failed to connet to server\n");
		char * error_buffer = malloc(256);
		sprintf(error_buffer,
			"Error: failed to connect to server at '\\0%s' ",
			soc.sun_path + 1);
		perror(error_buffer);
		return -1;
	}

	return 0;
}
int bridgeRun(bridge *b) {
	
	lan	*lcur;
	int	i, source;
	for(i = 0; i < b->numLans; i++) {
		lcur = &(b->lans[i]);
		memset(lcur->buf, 0x00, sizeof(lcur->buf));
		if( (lcur->bytes_read = read(lcur->sockfd, lcur->buf, sizeof(lcur->buf))) > 0) {	
			// Find the object associate with host
			fprintf(f,"Read %d bytes: %s\n", lcur->bytes_read, lcur->buf);
			source = getSource(lcur->buf);
			fprintf(f,"dec source is %04x\n", source);

		}
	}
/*	while( (bytes_read = read(b->sockfd, buf, sizeof(buf))) > 0) {
		bytes_written = write(b->sockfd, buf, bytes_read);
		if (bytes_written != bytes_read) {
			if (bytes_written > 0) {
				fprintf(stderr, "Error: Partial write");
			} else {
				perror("Write error");
				exit(-1);
			}
		}
		memset(buf, 0x00, sizeof(buf));
	}
*/
	return 0;
}

int findHost(bridge *b, short hostName) {
	int	i;
	
	for(i = 0; i < b->numHosts; i++) {
		if(host_list[i].name == hostName) {
			return i;
		}
	}

	// This means the host isn't in our current list
	return -1;
}

int addHost(bridge *b, int lanNum, int hostName) {
	host_list = (host *)realloc(host_list, sizeof(host_list) + sizeof(host));
	b->numHosts++;
	
	host_list[b->numHosts-1].lanNum = lanNum;
	host_list[b->numHosts-1].name = hostName;

	
	return 0;
}
int bridgeClose(bridge * b) {
	int 	i;
	
	for(i = 0; i < b->numLans; i++) {
		close(b->lans[i].sockfd);
	}
	return 0;
}
