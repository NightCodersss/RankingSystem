CXX=g++-5
CXXFLAGS=-c -std=c++14
LDFLAGS=-lboost_system -L../UbjsonCpp/src -lUbjsonCpp  
OBJECTS=main.o south_connection.o south_server.o 
EXECUTABLE=indexserver

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
