// strhelper.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strhelper.h"

unsigned int strlen_lan_name(const char * name) {
	return 1 + strlen(name + 1);
}

void strcpy_lan_name(char * dest, const char * src) {
	dest[0] = src[0];
	strcpy(&(dest[1]), &(src[1]));
}

char * strdup_lan_name(const char * name) {
	unsigned int malloc_size = strlen_lan_name(name) + 1;
	if (malloc_size == 1) { return NULL; }
	char * to_return = (char *) malloc(malloc_size);
	strcpy_lan_name(to_return, name);
	return to_return;
}

char * create_lan_name(const char * source_name) {
	char * to_return = (char *) malloc(strlen(source_name) + 2);
	to_return[0] = 0;
//	to_return[0] = '0';
	strcpy(to_return + 1, source_name);
	return to_return;
}

void print_lan_name(const char * name) {
	fprintf(f,"\\0%s", name + 1);
//	fprintf(f,"0%s", name + 1);
}
int getSource(char * buf) {
	char	*source;
	int	ret;

 	source = (char *)malloc(sizeof(char)*5);
	memcpy(source, &(buf[11]), 4*sizeof(char));
	sscanf(source, "%x", &(ret));
	return ret;
}
int getDest(char *buf) {
	char	*dest;
	int	ret;

 	dest = (char *)malloc(sizeof(char)*5);
	memcpy(dest, &(buf[25]), 4*sizeof(char));
	sscanf(dest, "%x", &(ret));
	return ret;
}
