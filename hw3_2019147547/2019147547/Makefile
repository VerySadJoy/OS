CC = g++
CFLAGS = -c -Wall
LDFLAGS =
SOURCES = project3.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = project3

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o $(EXECUTABLE)
