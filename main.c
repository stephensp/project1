
// main.c


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bridge.h"
#include "strhelper.h"

int main(int argc, char *argv[]) {

	bridge *b;
	int i;
	
	f = fopen("debug.txt", "w");
	if(f == NULL) {
		printf("Error opening debug file\n");
	}
	fprintf(f, "Hello world\n");
	// Check to make sure there are at least 2 arguements
	if(argc < 3) {
		fprintf(f,"Usage: 3700client <id> <LAN> [<LAN> [<LAN ...]]\n");
		return 0;
	}

	// Create a bridge instance
	b = malloc(sizeof(bridge));

	// Set some variables
	b->id = atoi(argv[1]);
	sscanf(argv[1], "%x", &(b->id));	

	b->numLans = 0;
	for(i = 2; i < argc; i++) {
		b->numLans++;
	}
	b->lans = (lan *)malloc(b->numLans * sizeof(lan));
	// Let's make this run with just 1 LAN for now
	for(i = 0; i < b->numLans; i++) {
		b->lans[i].name = create_lan_name(argv[2+i]);
		fprintf(f,"Setting lan name 0%s \n", b->lans[i].name);
	}

	fprintf(f,"Bridge %04x starting up\n", b->id);

	if(bridgeInit(b) == -1) {
		fprintf(f,"Error: Init failure\n");
	}

	// Now run!
	if(bridgeRun(b) == -1) {
		fprintf(f,"Error: Run failure\n");
	}

	// Clean up
	if (bridgeClose(b) == -1) {
		fprintf(f,"Error: Close failure\n");
	}
	
	free(b);

	return 0;
}
