CC=gcc
CFLAGS=-c -Wall 
INCLUDES= -I./jansson -I./include
LDFLAGS= -L./jansson/.libs/libjansson.a
SOURCES=./src/main.c ./src/bridge.c ./src/strhelper.c ./src/decodehelper.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=3700bridge


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean: 
	rm -f $(EXECUTABLE) *.o debug.txt 0#*
