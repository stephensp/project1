// client.c
// response for all client functions, c is the client structure passed to 
// all functions. Since it is often used I have left off documentation 
// in each function explation for it.

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "client.h"


int state = 0; 

// setPort()
// Sets port number from user input
// @param c client 
// @param port
void setPort(client *c, int port) {
	printf(" setting port to %d\n", port);
	c->port = port;
}


// setStudentID()
// Sets studentID from user input
// @param c client 
// @param studentID 
void setStudentID(client *c, int studentID) {
	printf(" setting studentID to %d\n", studentID);
	c->studentID = studentID;
}

// setHostname()
// Sets Hostname from user input
// @param c client 
// @param hostname 
void setHostname(client *c, char *hostname) {
	printf(" setting hostname to %s\n", hostname);
	c->hostname = hostname;
}

// clientRun()
// The main function which runs a while loop for sending and receiving
// messages from the server
// @param c client
void clientRun(client *c) {
	char	ascSol[7];
	char	*buffer, *write_buffer;
	ssize_t	buff_size;
	int	sol;

	if(clientInit(c) == -1) {
		// If an error occurs from clientInit close socket and exit
		// A more verbose error message will be printed in clientInit
		printf("Error detected, exiting... \n");
		close(c->sockfd);	
		return;
	}
	
	// If init was sucessful we can allocate our buffers
	c->buffer = (char *)malloc(BUFF_SIZE);
	c->write_buffer = (char *)malloc(BUFF_SIZE);

	buffer = c->buffer;
	write_buffer = c->write_buffer;

	// Send hello message
	strcpy(buffer, "cs3700spring2015 HELLO ");
	sprintf(&(buffer[strlen(buffer)]), "%d", c->studentID);
	buffer[strlen(buffer)] = 10; // Add \n to end

	printf("Writing hello message\n");
	buff_size = write(c->sockfd, buffer, strlen(buffer));

	// Run continuous loop until a BYE message is received from the 
	// server, SUCCCESS or state being set will cause the program to 
	// terminate
	while(1) {
		// Check if the previous message was the final message
		if(state == SUCCESS) {
			printf("%s\n", buffer);
			printf("Success...exiting program!\n");
			break;
		}

		// Check if the previous message caused a failure
		if(state == FAIL) {
			printf("Failure encountered on following message \n");
			printf("%s\n", buffer);
			break;
		}

		// Clear buffers, use two different buffers to make debugging
		// easier if a failure occur. We wouldn't want to overwrite
		// the string that caused our failure
		memset(buffer, 0, BUFF_SIZE);
		memset(write_buffer, 0, BUFF_SIZE);

		// Now listening 
		buff_size = read(c->sockfd, buffer, BUFF_SIZE); 	
		
		// Evaluate the expression in the string
		sol = clientMath(c);
		
		// Format our response string
		strcpy(write_buffer,"cs3700spring2015 ");	
		sprintf(&(write_buffer[17]), "%d", sol); // Convert int to asci
		write_buffer[strlen(write_buffer)] = 10; // Add \n to end

		// Now write our response
		buff_size = write(c->sockfd, write_buffer,strlen(write_buffer));
		
	}

	// Let's make sure to close the connection
	close(c->sockfd);	
	
	// Free some memory
	free(buffer);
	free(write_buffer);
}

// clientInit()
// Handles creating the socket and connecting to the server
// @param c client
// @return -1 on failure 0 on success
int clientInit(client *c) {
	struct 	sockaddr_in soc;
	struct 	hostent *host;
	int 	status;

	// Let's first create a socket
	c->sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(c->sockfd == -1) {
		printf("Error: socket returned failure\n");
		return -1;
	}
	
	// Now we need to get the host IP addr
	host = gethostbyname(c->hostname);
	if(host == NULL) {
		printf("Error: could not find host IP\n");
		return -1;
	}

	// Set socket info
	memcpy(&(soc.sin_addr.s_addr), host->h_addr, host->h_length);
	soc.sin_port = htons(c->port);
	soc.sin_family = AF_INET;

	// Next connect client socket to server socket
	status = connect(c->sockfd, (struct sockaddr *) &soc, sizeof(soc));
	
	if(status == -1) {
		printf("Error: failed to connect to server\n");
		return -1;
	}

	return 0;
}

// clientMath()
// Evaluates the math expression given by the server
// @param c client
// @return int solution to the expression
int clientMath(client *c) {
	// Now to parse the string in C, which will be ugly	
	char 	m1[] = "cs3700spring2015 ";
	char	m3[] = "STATUS ";
	char	p[3];
	char	*y1, *y2, *buffer;
	int 	l1, l2, l3, start, q1, q2;
	int	x1, x2, op, sol;
	int 	i;

	// Some initilizations
	buffer = c->buffer;
	l1 = strlen(m1);   	
	l2 = strlen(buffer);
	l3 = strlen(m3);
	y1 = (char *) malloc(1000);
	y2 = (char *) malloc(1000);
	memset(y1,0,1000);
	memset(y2,0,1000);

	// First check to make sure that the message is formatted correctly
	for(i = 0; i < (l1+l3); i++) {
		// Check that cs3700spring2015 is the beginning of the mes
		if(i < l1) {
			if(buffer[i] != m1[i]) {
				state = FAIL;
				printf("string does not match, ending\n");
				break;
			}
		}else {
			// Either this is the final message or it is badly
			// formatted
			if(buffer[i] != m3[i-l1]) {
				if(clientCheckSuc(c, &buffer[i])) {
					// We got a BYE message
					state = SUCCESS;
				}else{
					// We just got a bad format
					state = FAIL;
				}
				break;
			}
		}
	}
	start = i;	
	q1 = 0;
	q2 = 0;
	op = 0;
	
	// Now pull out the operators and the operands
	for(i = start; i < l2; i++) {
		// New line means the expression is done
		if(buffer[i] == 10) {
			break;
		}

		// If it is not a digit it must be the operator, valid 
		// operator will be checked later
		if((buffer[i] < 48) || (buffer[i] > 57)) {
			memcpy(p, &(buffer[i]) , 3);
			op = p[1];
			i = i+2;

		}else if(op == 0) {
			// Get the ascii characters of the first operand
			y1[q1] = buffer[i];
			q1++;
		}else {
			// Get the ascii characters of the second operand
			y2[q2] = buffer[i];
			q2++;	
		}


	}
	
	// Convert the ascii to int so we can math
	x1 = atoi(y1);
	x2 = atoi(y2);

	// Deallocation
	free(y1);
	free(y2);

	// Now check the operators and do math accordingly
	if(op == '+') {
		return (x1 + x2);
	}
	if(op == '-') {
		return (x1 - x2);
	}
	if(op == '*') { 
		return (x1 * x2);
	}
	if(op == '/') {
		return (x1 / x2);
	}else {
		if(state == RUNNING) {
			printf("Error: non valid operator\n");
			state = FAIL;
		}
		return 0;
	}
	


}
// clientCheckSuc()
// Checks thata SUCCCESS message was received and not a wrongly formatted
// message
// @param c client
// @param buffer to check against, will start at where success message should
// 	exists. 
// @return 1 for success, 0 for failure
int clientCheckSuc(client *c, char *buffer) {
	char	m[] = "BYE";
	ssize_t	x;
	int	i;

	x = strlen(buffer);	
	for(i = 0; i < 3  ; i++) {
		if(buffer[x-i-2] != m[2-i]) {
			return 0;
		}
	}
	return 1;
}
