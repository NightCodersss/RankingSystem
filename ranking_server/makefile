CXX=g++-5
INCLUDEPATH=-I../config_loader/ -I../UbjsonCpp/include/
LIBRARYPATH=-L../UbjsonCpp/src -L../config_loader
CXXFLAGS=-c -std=c++14 $(INCLUDEPATH)
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
