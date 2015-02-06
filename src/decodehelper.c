#include "bridge.h"
#include "decodehelper.h"
#include <jansson.h>
int jsonDecode(message m) {
	json_t		*jroot, *jsrc, *jdest , *jtype, *jdata, *jtmp;
	json_error_t	*jerror;
	char		*buf;
	int		i;

	jroot = json_loads(buf, 0, jerror);
	if(jroot == NULL) {
		printf("Json error has occured\n");
		perror("Json error\n");
		fflush(stdout);
	}
	
	// Go through ever element in array and check against key 
	for(i = 0; i < json_array_size(jroot); i++) {

		
		jtmp = json_array_get(jroot, i);
		
		jsrc 	= json_object_get(jtmp, "source");
		jdest 	= json_object_get(jtmp, "dest");
		jtype 	= json_object_get(jtmp, "type");
		jdata 	= json_object_get(jtmp, "message");

		if(jsrc == NULL || jdest == NULL || jtype == NULL || 
				jdata == NULL) {

			printf("Error seraching for JSON key\n");
			fflush(stdout);

			return -1;
		}

		printf("original: %s\n", buf);
		printf("src = %s\n", json_string_value(jsrc));
		printf("dest = %s\n", json_string_value(jdest));
		printf("type = %s\n", json_string_value(jtype));
		printf("data = %s\n", json_string_value(jdata));
	
		
	}

	return 0;

}
