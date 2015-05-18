CXX=g++-5
CXXFLAGS=-c -std=c++14
LDFLAGS=-lboost_system -L../UbjsonCpp/src -lUbjsonCpp -lpthread 
OBJECTS=main.o ranking_connection.o ranking_server.o 
EXECUTABLE=rankingserver

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
