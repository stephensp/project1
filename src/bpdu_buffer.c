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
		if(b->root->rec_port == i) {
			b->on_lans[i] = &(b->lans[i]);
			b->bpdu_buf_on[i] = b->bpdu_buf[i];
		}
		else if(isDesignatedPort(b, i)){
			printf("Designated port: %08x/%d\n", b->id, i);
			fflush(stdout);
			b->on_lans[i] = &(b->lans[i]);
			b->bpdu_buf_on[i] = b->bpdu_buf[i];
		}else {
			b->on_lans[i] = 0;
			b->bpdu_buf_on[i] = 0;
			printf("Disabled port: %08x/%d\n", b->id, i);
			fflush(stdout);
		}
	}

}

int isDesignatedPort(bridge *b, int port) {

	// Compare BPDU's
	if(b->bpdu_buf[port] == NULL) {
		// No BPDU for this port yet, assume designated
		printf("Enabling port %d because no BPDU\n", port);
		return 1;
	}
	if(b->root->cost < b->bpdu_buf[port]->cost) {
		printf("Enabling port %d because b->root->cost = %d and port->root->cost = %d\n", 
				port, b->root->cost, b->bpdu_buf[port]->cost);
		return 1;
	}else if(b->root->cost == b->bpdu_buf[port]->cost) {
		if(b->id < b->bpdu_buf[port]->bridgeid) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}
void newRoot(bridge *b, bpdu *newr) {

		if(newr->rootid < b->root->rootid) {
			// We have a new root
			b->root = newr;
			b->root->cost++;
			printf("Root port %08x/%d\n", newr->rootid, newr->rec_port);
			printf("Cost of root is %d\n", newr->cost);
		}
		if(newr->rootid == b->root->rootid) {
			if(newr->cost < b->root->cost) {
				b->root = newr;
				b->root->cost++;
				printf("Root port %08x/%d\n", newr->rootid, newr->rec_port);
				printf("Cost of root is %d\n", newr->cost);
			}
			if(newr->rec_port < b->root->rec_port) {
				b->root = newr;
				b->root->cost++;
				printf("Root port %08x/%d\n", newr->rootid, newr->rec_port);
				printf("Cost of root is %d\n", newr->cost);
			}
		}
		// If this BPDU is better than the current one on this port,
		// replace it
		if(b->bpdu_buf[newr->rec_port] == NULL) {
			b->bpdu_buf[newr->rec_port] = newr;
		} 
		else if(newr->rootid < b->bpdu_buf[newr->rec_port]->rootid) {
			printf("newr->rec_port is %d\n", newr->rec_port);
			fflush(stdout);
			b->bpdu_buf[newr->rec_port] = newr;
		} 
		else if(newr->rootid == b->bpdu_buf[newr->rec_port]->rootid) {
			if(newr->cost < b->bpdu_buf[newr->rec_port]->cost) {
				b->bpdu_buf[newr->rec_port] = newr;
			}else if(newr->cost == b->bpdu_buf[newr->rec_port]->cost) {
				if(newr->bridgeid < b->bpdu_buf[newr->rec_port]->bridgeid){
					b->bpdu_buf[newr->rec_port] = newr;
				}
			}
		}

		turnoffPorts(b);
}

