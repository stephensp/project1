// bridge.h
#ifndef BRIDGE_H
#define BRIDGE_H

typedef struct {
	char	buf[256];
	char 	*name;
	int	sockfd;
	int	bytes_read;
	int	bytes_written;
} lan;

typedef struct {
	lan	*lans;
	int 	id;
	int	numLans;
	int	numHosts;


} bridge;

typedef struct {
	int	time;
	int	lanNum;
	short	name;
} host;

host *host_list;

int bridgeInit(bridge *b);
int socketInit(lan *l);
int bridgeRun(bridge *b);
int findHost(bridge *b, short hostName);
int addHost(bridge *b, int lanNum, int hostName);
int bridgeClose(bridge * b);



#endif
