
#ifndef DECODEHELPER_H
#define DECODEHELPER_H

#include "bridge.h" 

int 		jsonDecode(packet *m);
bpdu*	 	decodeBpdu(packet *m);
const char* 	encodeBpdu(bridge *b);
void 		addPair(json_object *jobj, char *type, char *val);
int 		encodeMessage(packet *m);
int 		getValues(json_object *jobj, packet *m);
int 		setValues(json_object *val, char *key, packet *m);


#endif
