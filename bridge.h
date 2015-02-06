// bridge.h
#ifndef BRIDGE_H
#define BRIDGE_H

#define MAXFD	4681250		// /proc/sys/fs/file-max
#define	MAXBUF	256
typedef struct {
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
	fd_set	fdsoc;


} bridge;

typedef struct {
	int	time;
	lan	lanOn;
	short	name;
} host;

typedef struct {
	char	*buf;
	int	byte_written;
	int	bytes_read;
	short	src;
	short	dest;
} message;

host *host_list;

int bridgeInit(bridge *b);
int socketInit(lan *l);
int bridgeRun(bridge *b);
int waitMessage(bridge *b);
lan* findHost(short hostName);
int addHost(bridge *b, int lanNum, int hostName);
int writeToAllLans(bridge *b, char *buf, int bytes_read);
int bridgeClose(bridge * b);



#endif
