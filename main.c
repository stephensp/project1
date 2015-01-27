
// main.c


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include "bridge.h"

int main(int argc, char *argv[]) {

	bridge *b;
	int i;
	
	// Check to make sure there are at least 2 arguements
	if(argc < 3) {
		printf("Usage: 3700client <id> <LAN> [<LAN> [<LAN ...]]\n");
		return 0;
	}

	// Create a bridge instance
	b = malloc(sizeof(bridge));

	// Set some variables
	b->id = atoi(argv[1]);
	
	// Let's make this run with just 1 LAN for now
	b->lan = (char*)malloc(sizeof(argv[2]));

	strcpy(b->lan, argv[2]);
	
	printf("Bridge %d starting up\n", b->id);
	printf("Lan name %s \n", b->lan);
	// Clean up
	free(b);

	return 0;
}
