#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "bridge.h"
#include "bpdu_buffer.h"
#include "strhelper.h"
#include "decodehelper.h"

int bridgeInit(bridge *b) {
	// First let's set up the socket
	int status, i;

	for(i = 0; i < b->num_total_lans; i++) {
		status = socketInit(&(b->lans[i]));
		if(status == -1) {
			return status;
		}
	}

	b->numHosts = 0;
	
	FD_ZERO(&b->fdsoc); 

	// Add each LAN socket 
	for(i = 0; i < b->num_total_lans; i++ ) {
		FD_SET(b->lans[1].sockfd, &(b->fdsoc));
	}

	// Set itself to root
	b->root = (bpdu *)malloc(sizeof(bpdu));
	b->root->rootid = b->id;
	b->root->cost = 0;
	b->root->port = -1; // No port for now
	create_bpdubuffer(b);
	return 0;
}

int socketInit(lan *l) {
	struct	sockaddr_un soc;
	int	status;

	memset(&soc, 0, sizeof(soc));
	l->sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if(l->sockfd == -1) {
		char * error_buffer = malloc(256);
		sprintf(error_buffer,
			"Error: failed to create socket.");
		perror(error_buffer);
		fflush(stdout);
		return -1;
	}

	soc.sun_family = AF_UNIX;
	
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

	return 0;
}
int bridgeRun(bridge *b) {
	char	buf[MAXBUF];
	packet  *p;
	lan	*lanCur;
	int	status;
	int	i;
	

	p = (packet *)malloc(sizeof(packet));
	p->buf = buf;
	status = 0;
	memset(buf, 0, MAXBUF);

	sendBpdu(b);
	while(1) {	
		status = waitPacket(b);
		while(status == -2) {
			// Try one more time
			status = waitPacket(b);
//			if(status == -2) {
//			}
		}
		memset(buf, 0, MAXBUF);
		for(i = 0; i < b->num_total_lans; i++) {
			lanCur = &(b->lans[i]);
			if(FD_ISSET(lanCur->sockfd, &b->fdsoc) != 0){
				p->port = lanCur->port;
				p->bytes_read = read(lanCur->sockfd, buf, 
						MAXBUF);
				printf( "%d bytes read from lan %s\n", 
						p->bytes_read,
						&(lanCur->name[1]));
				printf( "Message read: %s\n", buf);
				printf( "Message port is: %d\n", p->port);
				fflush(stdout);

				// Let's just write to all LANS for now
				if(jsonDecode(p) != 0) {
					printf("Failing decoding messsage,	message dropped\n");
					break;
				}
				if(p->type == BPDU) {
					printf("found a BPDU\n");
					if(updateBpdu(b, p) != 0) {
						printf("Error update BPDU\n");
					}

				}
				if(p->type == DATA) {
					printf("found a data packet\n");
					if(sendPacket(b, p) != 0) {
						printf("Error sending\n");
					}
				//	printHostlist(b);
				}
			}
		}
	}
	free(p);
	fflush(stdout);
	return 0;
}
int updateBpdu(bridge *b, packet *p) {

	bpdu* 	newr;
	int	status = 0;

	newr = decodeBpdu(p);
//	length = sizeof(b->bpdu_buf)/sizeof(bpdu*);
	printf("rootid %08x, cost %d, port %d\n", newr->rootid, newr->cost,
			newr->port);

	fflush(stdout);
	
	if(newr == NULL) {
		printf("Error decoding BPDU\n");
		return -1;
	}
	status = add_bpdubuffer(b, newr);
	if(newr->rootid < b->root->rootid) {
		// We have a new root
		newRoot(b, newr);
	}
	if(newr->rootid == b->root->rootid) {
		if(newr->cost < b->root->cost) {
			newRoot(b, newr);
		}
		if(newr->port < b->root->port) {
			newRoot(b, newr);
		}
	}
	return status;

}

int sendBpdu(bridge *b) {
	const char* buf;
	int i;

	buf = encodeBpdu(b);

	for(i = 0; i < b->num_total_lans; i++) {
		write(b->lans[i].sockfd, buf, MAXBUF);
	}
	
	return 0;

}

int waitPacket(bridge *b) {
	struct	timeval	timeout;
	int	i, rs; 

	// Set time limit
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;
	

	FD_ZERO(&b->fdsoc); 

	// Add each LAN socket 
	for(i = 0; i < b->num_total_lans; i++ ) {
		FD_SET(b->lans[i].sockfd, &(b->fdsoc));
	}

	rs = select(sizeof(b->fdsoc)*MAXFD, &b->fdsoc, NULL, NULL, 
			&timeout);

	if(rs == -1) {
		printf( "Error: Select error\n");
		perror("Select error");
		fflush(stdout);
		return -1;
	}
	if(rs == 0) {
		return -2;
	}

	return 0;

}

int sendPacket(bridge *b, packet *p) {
	writeToAllLans(b, p);
//	lan 	*dest;
//	int	bytes_written;
//
//	dest = findHost(p->dest);
//	if(dest == NULL) {
//		// We don't know where host is, send on all LANS
//		printf("Writing to all LANS\n");
//		if(writeToAllLans(b, p) != 0) {
//			printf("Error: from writing to all LANS\n");
//
//		}
//
//	}else{
//		
//		bytes_written = write(dest->sockfd, p->buf, p->bytes_read);
//		printf("bytes_read = %d, bytes_written = %d\n", p->bytes_read, bytes_written);
//		fflush(stdout);
//		printf( "Writing %s to %s \n", p->buf, &(dest->name[1]));
//		fflush(stdout);
//	}
//		
//
//	if(findHost(p->src) == NULL) {
//		addHost(b, p->port, p->src); 
//	}
	return 0;

}
lan* findHost(bridge *b, int hostName) {
	int	length, i;

	length = b->numHosts;

	for(i = 0; i < length; i++) {
		printf("checking %d against %d\n", host_list[i].name, hostName);
		if(host_list[i].name == hostName) {
			return &(host_list[i].lanOn);
		}
	}

	// This means the host isn't in our current list
	return NULL;
}

int addHost(bridge *b, int port, int hostName) {
	host_list = (host *)realloc(host_list, sizeof(host_list) + 
			sizeof(host));
	b->numHosts++;
	
	printf("Adding host: %d to lan%s\n", hostName, &(b->lans[port].name[1])); 
	host_list[b->numHosts-1].lanOn = b->lans[port];
	host_list[b->numHosts-1].name = hostName;
	host_list[b->numHosts-1].port = port;
	

	
	return 0;
}

void printHostlist(bridge* b) {
	int i;

	printf("***********Host list is*********\n");
	for(i = 0; i < b->numHosts; i++) {
		printf("Host: %08x on lan %s, portnum %d\n", host_list[i].name, 
				&(host_list[i].lanOn.name[1]), 
				host_list[i].port);
	}
	fflush(stdout);

}
int writeToAllLans(bridge *b, packet *p) {
	int 	bytes_written, bytes_read;
	int 	i;

	bytes_read = p->bytes_read;
	
	for(i = 0; i < b->num_total_lans; i++) {
		if(i == p->port) {
			continue;	
		}
		bytes_written = write(b->lans[i].sockfd, p->buf, bytes_read);
		printf("bytes_read = %d, bytes_written = %d\n", bytes_read, bytes_written);
		fflush(stdout);
		printf( "Writing %s to %s \n", p->buf, &(b->lans[i].name[1]));
		fflush(stdout);
		if(bytes_written != bytes_read) {
			if (bytes_written > 0) {
				printf( "Error: Partial write"); fprintf(stderr, "Error: Partial write");
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
	
	printf("Closing sockets\n");
	fflush(stdout);
	for(i = 0; i < b->num_total_lans; i++) {
		close(b->lans[i].sockfd);
	}

	free(b->root);
	return 0;
}
