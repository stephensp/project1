CC=gcc
CFLAGS=-c -Wall 
INCLUDES= -I./jansson -I./include
LDFLAGS=  -L./jansson/.libs/libjansson.a -static
SOURCES=./src/main.c ./src/bridge.c ./src/strhelper.c ./src/decodehelper.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=3700bridge


all: $(SOURCES) $(EXECUTABLE)
 
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(INCLUDE) $(OBJECTS) ./jansson/.libs/libjansson.a -o $@
 
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@
 
clean: 
	rm -f $(EXECUTABLE) *.o debug.txt 0#*
