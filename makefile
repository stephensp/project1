CC=gcc
CFLAGS=-c 
LDFLAGS=
SOURCES=main.c bridge.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=3700bridge


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.oo:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm -f $(EXECUTABLE) *.o
