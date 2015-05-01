CXX=g++-5
CXXFLAGS=-c -std=c++14
LDFLAGS=-lboost_system
OBJECTS=main.o index_connection.o index_server.o
EXECUTABLE=indexserver

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
