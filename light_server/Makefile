CC=$(CROSS_COMPILE)g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=-pthread
SOURCES=main.cpp Driver.cpp LightBoard.cpp LightsCore.cpp PatternRunner.cpp SocketServer.cpp SocketTester.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lights

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f light *.o
