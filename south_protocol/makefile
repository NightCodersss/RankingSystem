CXX=g++-5
CXXFLAGS=-c -DBOOST_LOG_DYN_LINK -std=c++14
LDFLAGS=-lboost_system -lboost_log -lboost_thread -L../UbjsonCpp/src -lUbjsonCpp -lpthread 
OBJECTS=main.o south_connection.o south_server.o 
EXECUTABLE=southserver

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
