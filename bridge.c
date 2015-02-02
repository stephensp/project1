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
	struct sockaddr_un soc;
	int length, status, i;

	b->sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if(b->sockfd == -1) {
		printf("Error: socket returned failure\n");
		return -1;
	}

	soc.sun_family = AF_UNIX;

	strcpy_lan_name(soc.sun_path, b->lan);
//	memcpy(soc.sun_path, b->lan, length);
	printf("soc.sun_path: 0%s\n", soc.sun_path);

	status = connect(b->sockfd, (struct sockaddr *) &soc, sizeof(soc));
	if(status == -1) {
	//	printf("Error: failed to connect to server\n");
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
	char buf[256];
	memset(buf, 0x00, sizeof(buf));
	int bytes_read, bytes_written;
	while( (bytes_read = read(b->sockfd, buf, sizeof(buf))) > 0) {
		printf("Read %d bytes: %s\n", bytes_read, buf);
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

	return 0;
}

int bridge_close(bridge * b) {
	close(b->sockfd);
	return 0;
}
