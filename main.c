
// main.c


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bridge.h"
#include "strhelper.h"

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
	sscanf(argv[1], "%x", &(b->id));	
	// Let's make this run with just 1 LAN for now
	b->lan = create_lan_name(argv[2]);
//	b->lan = (char*)malloc(sizeof(argv[2])+2);



	printf("Bridge %04x starting up\n", b->id);
	printf("Lan name 0%s \n", b->lan + 1);

	if(bridgeInit(b) == -1) {
		printf("Error: Init failure\n");
	}

	// Now run!
	if(bridgeRun(b) == -1) {
		printf("Error: Run failure\n");
	}

	// Clean up
	if (bridge_close(b) == -1) {
		printf("Error: Close failure\n");
	}
	
	free(b);

	return 0;
}
