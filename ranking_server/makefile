CXX=g++-5
INCLUDEPATH=-I$(PROJECTPATH)/config_loader/ -I$(PROJECTPATH)/UbjsonCpp/include/ -I$(PROJECTPATH)/
LIBRARYPATH=-L$(PROJECTPATH)/UbjsonCpp/src -L$(PROJECTPATH)/config_loader
CXXFLAGS=-c -g -std=c++14 $(INCLUDEPATH)
LDFLAGS=-lboost_system $(LIBRARYPATH) -lUbjsonCpp -lpthread -lconfig_loader
OBJECTS=main.o ranking_connection.o ranking_server.o
EXECUTABLE=rankingserver

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
