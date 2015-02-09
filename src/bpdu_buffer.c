#include "bpdu_buffer.h"
#include <stdio.h>
#include <string.h>
#include "bridge.h"
#include <sys/time.h>

void create_bpdubuffer(bridge *b) {

	b->bpdu_buf = (bpdu **) malloc(sizeof(bpdu *) * b->num_total_lans);
	b->bpdu_buf_on = (bpdu **) malloc(sizeof(bpdu *) * b->num_total_lans);

	memset(b->bpdu_buf, 0, sizeof(bpdu *) * b->num_total_lans);
	memset(b->bpdu_buf_on, 0, sizeof(bpdu *) * b->num_total_lans);
	
}

int add_bpdubuffer(bridge *b, bpdu *newr) {

//	gettimeofday(&(newr->time_added), NULL);
	b->bpdu_buf[newr->rec_port] = newr;
	return 0;
}

void turnoffPorts(bridge *b) {
	int i;
	
	for(i = 0; i < b->num_total_lans; i++) {
		if(b->root->port == i) {
			b->bpdu_buf_on[i] = b->bpdu_buf[i];
		}
		else if(isDesignatedPort(b, i)){
			b->bpdu_buf_on[i] = b->bpdu_buf[i];
		}else {
			b->bpdu_buf_on[i] = 0;
			printf("Disabled port: %08x/%d\n", b->id, i);
			fflush(stdout);
		}
	}

}

int isDesignatedPort(bridge *b, int port) {

	// Compare BPDU's
	printf("port is %d\n", port);
	fflush(stdout);
	printf("b->root->cost = %d\n", b->root->cost);
	fflush(stdout);
	printf("port->cost is %d\n", b->bpdu_buf[port]->cost);
	if(b->root->cost < b->bpdu_buf[port]->cost) {
		return 1;
	}else if(b->root->cost == b->bpdu_buf[port]->cost) {
		if(b->root->rootid < b->bpdu_buf[port]->rootid) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}
void newRoot(bridge *b, bpdu *newr) {

		b->root = newr;
		printf("Root port %08x/%d\n", newr->rootid, newr->rec_port);
		fflush(stdout);
		turnoffPorts(b);
}

