CXX=g++-5
INCLUDEPATH=-I$(PROJECTPATH)/config_loader/ -I$(PROJECTPATH)/UbjsonCpp/include/
LIBRARYPATH=-L$(PROJECTPATH)/UbjsonCpp/src -L$(PROJECTPATH)/config_loader
CXXFLAGS=-g -c -std=c++14 $(INCLUDEPATH)
LDFLAGS=-lboost_system $(LIBRARYPATH) -lUbjsonCpp -lpthread -lconfig_loader
OBJECTS=main.o index_connection.o index_server.o
EXECUTABLE=indexserver

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
