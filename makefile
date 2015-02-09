CC=gcc
CFLAGS=-c -Wall 
INCLUDES= -I./json-c -I./include
LDFLAGS=  -L./jansson/.libs/libjansson.a -static
SOURCES=./src/main.c ./src/bridge.c ./src/strhelper.c ./src/decodehelper.c ./src/bpdu_buffer.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=3700bridge


all: $(SOURCES) $(EXECUTABLE)
 
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(INCLUDE) $(OBJECTS) ./json-c/.libs/libjson-c.a -o $@
 
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@
 
clean: 
	rm -f $(EXECUTABLE) ./src/*.o debug.txt 0#*
