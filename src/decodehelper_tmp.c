#include <string.h>
#include <stdio.h>
//#include <jansson.h>
#include <json.h>
#include "decodehelper.h"
#include "bridge.h"

int jsonDecode(message *m) {
	json_object *jobj = json_tokener_parse(m->buf);

	enum json_type type;
	type = json_object_get_type(jobj); /*Getting the type of the json object*/
	switch (type) {
		case json_type_null:
			printf("json type null\n");
		case json_type_object:
			printf("json type object\n");
		case json_type_array:
			printf("json type array\n");
		case json_type_boolean: 
			printf("json_type_booleann ");
			printf("value: %sn", json_object_get_boolean(jobj)? 
					"true\n": "false\n");
					break;
		case json_type_double: 
			printf("json_type_double ");
		        printf("value: %lfn\n", json_object_get_double(jobj));
				       break;
		case json_type_int: 
				    printf("json_type_intn ");
				    printf("value: %dn \n", 
					json_object_get_int(jobj));
				    break;
		case json_type_string: 
			printf("json_type_string\n");
	}
	fflush(stdout);
	//	json_t		*jroot;
	//	json_t		*jsrc, *jdest , *jtype, *jdata, *jtmp;
//	json_error_t	jerror;
//	int		i;
//
//
////	jroot = json_loads(m->buf, 0, &jerror);
//	jroot = json_loads(m->buf, JSON_DECODE_ANY, &jerror);
//
//	if(jroot == NULL) {
//		printf("Json error has occured\n");
//		fflush(stdout);
//		perror("Json error\n");
//		fflush(stdout);
//		return -1;
//	}
//	
//	printf("sizeof jroot is %lu\n", json_array_size(jroot));
//	printf("bytes %d message is %s\n",m->bytes_read, m->buf);
//	fflush(stdout);
//	
//	// Go through ever element in array and check against key 
//	for(i = 0; i < json_array_size(jroot); i++) {
//
//		
//		jtmp = json_array_get(jroot, i);
//		
//		jsrc 	= json_object_get(jtmp, "source");
//		jdest 	= json_object_get(jtmp, "dest");
//		jtype 	= json_object_get(jtmp, "type");
//		jdata 	= json_object_get(jtmp, "message");
//
//		if(jsrc == NULL || jdest == NULL || jtype == NULL || 
//				jdata == NULL) {
//
//			printf("Error seraching for JSON key\n");
//			fflush(stdout);
//
//			return -1;
//		}
//
//		printf("original: %s\n", m->buf);
//		printf("src = %s\n", json_string_value(jsrc));
//		printf("dest = %s\n", json_string_value(jdest));
//		printf("type = %s\n", json_string_value(jtype));
//		printf("data = %s\n", json_string_value(jdata));
//		fflush(stdout);
//		
//	}

	return 0;

}
