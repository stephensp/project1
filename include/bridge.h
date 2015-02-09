// bridge.h
#ifndef BRIDGE_H
#define BRIDGE_H

#include <netdb.h>
#include <json.h>
#include <sys/time.h>


#define MAXFD	4681250		// /proc/sys/fs/file-max
#define	MAXBUF	256

enum type_t {BPDU , DATA};

typedef struct {
	char 	*name;
	int	sockfd;
	int	bytes_read;
	int	bytes_written;
	int	port;
} lan;

typedef struct {
	int		rootid;
	int		cost;
	int		port;
	int		rec_port; // Port the BPDU came in on 
	struct timeval	time_added; 
} bpdu;

typedef struct {
	lan	*lans;
	lan	**on_lans;
	int 	id;
	int	num_total_lans;
	int	numHosts;
	fd_set	fdsoc;
	bpdu	*root;
	bpdu	**bpdu_buf;
	bpdu	**bpdu_buf_on;

} bridge;


typedef struct {
	int	time;
	lan	lanOn;
	int	name;
	int	port;
} host;

typedef struct {
	char		*buf;
	int		byte_written;
	int		bytes_read;
	int		src;
	int		dest;
	enum type_t 	type;
	char		message[256];
	int		port;
} packet;


int 	bridgeInit(bridge *b);
int 	socketInit(lan *l);
int 	bridgeRun(bridge *b);
int	updateBpdu(bridge *b, packet *m);
int	sendBpdu(bridge *b);
int 	waitPacket(bridge *b);
int	sendPacket(bridge *b, packet *p);
void	printHostlist(bridge *b);
int	writeToAllOnLans(bridge *b, packet *p);
int 	writeToAllLans(bridge *b, packet *p);
int 	bridgeClose(bridge * b);


int host_list[0xFFFF];


#endif
