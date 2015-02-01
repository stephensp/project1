// client.h
// Header file for all client functions and client struct

typedef struct {
	char *buffer;
	char *write_buffer;
	char *hostname;
	int port;
	int studentID;
	int sockfd;
} client;

void setPort(client *c, int port);
void setStudentID(client *c, int studentID);
void setHostname(client *c, char *hostname);
void clientRun(client *c);
int clientInit(client *c); 
int clientMath(client *c);
int clientCheckSuc(client *c, char *buffer);

#define SUCCESS		1
#define FAIL		-1	
#define RUNNING		0 
#define BUFF_SIZE	256

