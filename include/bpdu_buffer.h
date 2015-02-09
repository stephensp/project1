
#ifndef BPDU_BUFFER_H
#define BPDU_BUFFER_H

#include "bridge.h" 

void create_bpdubuffer(bridge *b);
int add_bpdubuffer(bridge *b, bpdu *newr);
int isitaNewRoot(bridge *b, int port);
int isDesignatedPort(bridge *b, int port);
void newRoot(bridge *b, bpdu *newr);


#endif
