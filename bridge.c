#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "bridge.h"

int bridgeInit(bridge *b) {
	// First let's set up the socket
	struct sockaddr_un soc;
	int sockfd, length, status, i;

	sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if(sockfd == -1) {
		printf("Error: socket returned failure\n");
		return -1;
	}

	soc.sun_family = AF_UNIX;

	length = 1 + strlen(&(b->lan[1]));
	memcpy(soc.sun_path, b->lan, length);
	printf("soc.sun_path: %s\n", soc.sun_path);

//	for(i = 0; i < length; i++) {
//		printf("%d ", soc.sun_path[i]);
//	}
//	printf("\n");
	status = connect(sockfd, (struct sockaddr *) &soc, sizeof(soc));
	if(status == -1) {
		printf("Error: failed to connect to server\n");
		return -1;
	}
	
	close(sockfd);
	return 0;
}
int bridgeRun(bridge *b) {

	return 0;
}
