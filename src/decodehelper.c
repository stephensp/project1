#include <string.h>
#include <stdio.h>
//#include <jansson.h>
#include <json.h>
#include "decodehelper.h"
#include "bridge.h"

int jsonDecode(packet *m) {
	json_object *jobj = json_tokener_parse(m->buf);

	int status = getValues(jobj, m);

	fflush(stdout);
	return status;

}

bpdu* decodeBpdu(packet *m) {
	
	enum 	json_type type;
	bpdu	 *newbpdu;

	newbpdu = (bpdu *)malloc(sizeof(bpdu));

	printf("decoding BPDU\n");
	json_object_object_foreach(m->message, key, val) {
		printf("key is %s\n", key);
		type = json_object_get_type(val);
		if(strcmp(key, "root") == 0) {
			if(type == json_type_string) {
				sscanf(json_object_get_string(val), "%x", 
						&(newbpdu->rootid));
			}else {
				printf("Error: bpdu rootid bad json type\n");
				return NULL; 
			}
		}
		if(strcmp(key, "cost") == 0) {
			if(type == json_type_string) {
				sscanf(json_object_get_string(val), "%x", 
						&(newbpdu->cost));
			}else {
				printf("Error: bpdu cost bad json type\n");
				return NULL;
			}
		}
		if(strcmp(key, "cost") == 0) {
			if(type == json_type_string) {
				sscanf(json_object_get_string(val), "%x", 
						&(newbpdu->cost));
			}else {
				printf("Error: bpdu cost bad json type\n");
				return NULL;
			}
		}
				
	}
	return newbpdu;
}

const char* encodeBpdu(bridge *b) {
	
	char type[MAXBUF];
	char val[MAXBUF];
	json_object *jobj = json_object_new_object();

	memset(&type, 0, MAXBUF);
	memset(&val, 0, MAXBUF);

	strcpy(type, "source");
	sprintf(val, "%x", b->id);
	addPair(jobj, type, val);
	
	strcpy(type, "dest");
	strcpy(val, "ffff");
	addPair(jobj, type, val);
	
	strcpy(type, "type");
	strcpy(val, "bpdu");
	addPair(jobj, type, val);

	// Message is a bit more complex
	// First we will make a json object of it, then convert it to a
	// string before adding it into the whole message object
	json_object *message = json_object_new_object();

	strcpy(type, "id");
	sprintf(val, "%x", b->id);
	addPair(message, type, val);
	
	strcpy(type, "root");
	sprintf(val, "%x", b->root->rootid);
	addPair(message, type, val);

	strcpy(type, "cost");
	sprintf(val, "%x", b->root->cost);
	addPair(message, type, val);

	strcpy(type, "port");
	sprintf(val, "%x", b->root->port);
	addPair(message, type, val);

	strcpy(val ,json_object_get_string(message));
	strcpy(type, "message");
	addPair(jobj, type, val);

	return json_object_get_string(jobj);
	
}

void addPair(json_object *jobj, char *type, char *val){

	json_object *jstring = json_object_new_string(val);
	json_object_object_add(jobj, type , jstring);

}

int getValues(json_object *jobj, packet *m) {
	int 	status = -1;
	enum 	json_type type;

	json_object_object_foreach(jobj, key, val) {

		fflush(stdout);
		type = json_object_get_type(val);
		if(type == json_type_string) {
			status = setValues(val, key, m);
		}

		if(type == json_type_object) {
			status = setValues(val, key, m);
		}
	}
	return status;

}


int setValues(json_object *val, char * key, packet *m) {

	enum json_type type;
	type = json_object_get_type(val);

	if(strcmp(key, "source") == 0) {
		if(type == json_type_string) {
			sscanf(json_object_get_string(val), "%x", &(m->src));
			printf("m->src is %08x\n", m->src);
			return 0;
		}else {
			printf("Error: source val is of wrong json type\n");
			return -1;
		}
	}

	if(strcmp(key, "dest") == 0) {
		if(type == json_type_string) {
			sscanf(json_object_get_string(val), "%x", &(m->dest));
			printf("m->dest is %08x\n", m->dest);
			return 0;
		}else {
			printf("Error: dest val is of wrong json type\n");
			return -1;
		}
	}

	if(strcmp(key, "type") == 0) {
		if(type == json_type_string) {
			if(strcmp(json_object_get_string(val), "bpdu") == 0) {
				printf("setting m->type to bpdu\n");
				m->type = BPDU;
				return 0;
			}
			if(strcmp(json_object_get_string(val), "data") == 0) {
				printf("setting m->type to data\n");
				m->type = DATA;
				return 0;
			}
			printf("Error: type is an unknown\n");
			return -1;
		}else {
			printf("Error: type val is of wrong json type\n");
			return -1;
		}
	}
	
	if(strcmp(key, "message") == 0) {
		if(type == json_type_object) {
			m->message = val;
			return 0;
		}else {
			printf("Error: message val is of wrong json type\n");
			return -1;
		}
	}
		
	return 0;
}


