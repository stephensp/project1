// bridge.h
#ifndef BRIDGE_H
#define BRIDGE_H

typedef struct {
	int id;
	char *lan;
} bridge;


int  bridgeInit(bridge *b);
int  bridgeRun(bridge *b);


#endif
