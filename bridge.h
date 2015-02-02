// bridge.h
#ifndef BRIDGE_H
#define BRIDGE_H

typedef struct {
	int id;
	char *lan;
	int sockfd;
} bridge;


int  bridgeInit(bridge *b);
int  bridgeRun(bridge *b);
int bridge_close(bridge * b);


#endif
