#CXX=g++-5
INCLUDEPATH=-I$(PROJECTPATH)/config_loader/ -I$(PROJECTPATH)/UbjsonCpp/include/ -I$(PROJECTPATH)
LIBRARYPATH=-L$(PROJECTPATH)/UbjsonCpp/src -L$(PROJECTPATH)/config_loader
CXXFLAGS=-g -DBOOST_LOG_DYN_LINK -c -std=c++14 $(INCLUDEPATH)
LDFLAGS=-lboost_log -lboost_system -lboost_thread $(LIBRARYPATH) -lUbjsonCpp -lpthread -lconfig_loader
OBJECTS=main.o index_connection.o index_server.o
EXECUTABLE=indexserver

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
