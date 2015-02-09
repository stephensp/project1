
// main.c


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "bridge.h"
#include "strhelper.h"

int main(int argc, char *argv[]) {

	bridge *b;
	int i;
	
	printf("Hello world\n");

	// Check to make sure there are at least 2 arguements
	if(argc < 3) {
		printf( "%d parameters passed\n", argc);
		printf("Usage: 3700client <id> <LAN> [<LAN> [<LAN ...]]\n");
		fflush(stdout);
		return 0;
	}

	// Create a bridge instance
	b = malloc(sizeof(bridge));

	// Set some variables
	sscanf(argv[1], "%x", &(b->id));	

	b->num_total_lans = 0;
	for(i = 2; i < argc; i++) {
		b->num_total_lans++;
	}

	b->lans = (lan *)malloc(b->num_total_lans * sizeof(lan));

	// Add each lan to bridge structure and give each one a port num
	for(i = 0; i < b->num_total_lans; i++) {
		b->lans[i].name = malloc(sizeof(argv[2+1]));
		b->lans[i].name = create_lan_name(argv[2+i]);
		b->lans[i].port = i;
	}

	printf("Bridge %04x starting up\n", b->id);

	fflush(stdout);
	if(bridgeInit(b) == -1) {
		printf("Error: Init failure\n");
	}

	// Now run!
	if(bridgeRun(b) == -1) {
		printf("Error: Run failure\n");
	}

	// Clean up
	if (bridgeClose(b) == -1) {
		printf("Error: Close failure\n");
      	}

	fflush(stdout);

	free(b);

	fclose(f);

	return 0;
}
