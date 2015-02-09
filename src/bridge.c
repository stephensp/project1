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
	

	printf("New root: %08x/%08x\n", b->id, b->root->rootid);

	// Init on_lans array , and turn them all on for now
	b->on_lans = (lan **)malloc(sizeof(lan *) * b->num_total_lans);
	for(i = 0; i < b->num_total_lans; i++) {
		b->on_lans[i] = &(b->lans[i]);
	}

	// Init host_list
	memset(host_list, -1, 0xffff * sizeof(int));
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
	status = 0;
	memset(p, 0, sizeof(packet));
	p->buf = buf;

	sendBpdu(b);
	while(1) {	
		memset(buf, 0, MAXBUF);
		memset(p->message, 0, MAXBUF);
		status = waitPacket(b);
		while(status == -2) {
			status = waitPacket(b);
		}
		memset(buf, 0, MAXBUF);
		for(i = 0; i < b->num_total_lans; i++) {
			lanCur = &(b->lans[i]);
			if(FD_ISSET(lanCur->sockfd, &b->fdsoc) != 0) {
				p->port = lanCur->port;
				p->bytes_read = read(lanCur->sockfd, buf, 
						MAXBUF);
		//		printf("Read message %s\n", p->buf);
				if(jsonDecode(p) != 0) {
					printf("Failing decoding messsage,	message dropped\n");
					break;
				}
				if(p->type == BPDU) {
					if(updateBpdu(b, p) != 0) {
						printf("Error update BPDU\n");
					}

				}
				if(p->type == DATA) {
					// Check if we shoould be listneing on
					// this port
					if(b->on_lans[i] == 0) { continue; } 

					printf("Received %s on port %d from source %08x to dest %08x\n",
							p->message, p->port, 
							p->src, p->dest);
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

	printf("BPDU read on port %d is %s\n", p->port, p->buf);
	printf("rootid = %08x, bridgeid = %08x, cost = %d, rec_port = %d, port = %d\n", 
			newr->rootid, newr->bridgeid, newr->cost, newr->rec_port, newr->port);
	fflush(stdout);
	
	if(newr == NULL) {
		printf("Error decoding BPDU\n");
		return -1;
	}
	status = add_bpdubuffer(b, newr);

	newRoot(b, newr);

	return status;

}

int sendBpdu(bridge *b) {
	const char* buf;
	int i;


	for(i = 0; i < b->num_total_lans; i++) {
		buf = encodeBpdu(b, i);
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
	lan 	*dest;
	int	dest_port;

	dest_port = host_list[p->dest];
	if(dest_port == -1) {
		// We don't know where host is, send on all LANS
		if(writeToAllOnLans(b, p) != 0) {
			printf("Error: from writing to all LANS\n");

		}

	}else{

		dest = &(b->lans[dest_port]);	
		write(dest->sockfd, p->buf, p->bytes_read);
		printf("Forwarding message %s to port %d\n", p->message, 
				dest_port);
		fflush(stdout);
	}
		

	// Overwrite whatever was there, just to make sure it is uptodate
	host_list[p->src] = p->port;
	
//	printSpanningTree(b);

//	printHostlist(b);

	return 0;

}
void printSpanningTree(bridge *b) {
	int i;
	printf("\n\n\n************Printing Spanning Tree for Bridge %08x****************\n", b->id);
	for(i = 0; i < b->num_total_lans; i++){
		if(b->on_lans[i] != 0) {
			printf("port %i is on\n", i );
		}else {
			printf("port %i is off\n", i);
		}
	}
	fflush(stdout);

}
void printHostlist(bridge *b) {
	int i;
	printf("***********Printing Host List********************\n");
	printf("num_total_lans = %d\n", b->num_total_lans);
	for(i = 0; i < 0xffff; i++) {
		if(host_list[i] != -1) {
			printf("host: %04x on port %d\n", i, host_list[i]);
		}
	}
	fflush(stdout);
}

int writeToAllOnLans(bridge *b, packet *p) {
	int	bytes_written;	
	int	i;

	printf( "Broadcasting message %s to all ports\n", p->buf);
	for(i = 0; i < b->num_total_lans; i++) {
		if(i == p->port) {
			continue;	
		}
		if(b->on_lans[i] == NULL) {
			continue;
		}
		bytes_written = write(b->on_lans[i]->sockfd, p->buf, p->bytes_read);
		fflush(stdout);
		if(bytes_written != p->bytes_read) {
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
int writeToAllLans(bridge *b, packet *p) {
	int 	bytes_written, bytes_read;
	int 	i;

	bytes_read = p->bytes_read;
	
	for(i = 0; i < b->num_total_lans; i++) {
		if(i == p->port) {
			continue;	
		}
		bytes_written = write(b->lans[i].sockfd, p->buf, bytes_read);
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
	
	fflush(stdout);
	for(i = 0; i < b->num_total_lans; i++) {
		close(b->lans[i].sockfd);
	}

	free(b->root);
	return 0;
}
