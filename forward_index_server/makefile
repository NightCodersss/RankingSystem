#CXX=g++-5
#INCLUDEPATH=-I$(PROJECTPATH)/config_loader/ -I$(PROJECTPATH)/UbjsonCpp/include/ -I$(PROJECTPATH)
#LIBRARYPATH=-L$(PROJECTPATH)/UbjsonCpp/src -L$(PROJECTPATH)/config_loader
CXXFLAGS=-g -DBOOST_LOG_DYN_LINK -c -std=c++14 $(INCLUDEPATH)
LDFLAGS=-lboost_log -lboost_system -lboost_thread $(LIBRARYPATH) -lUbjsonCpp -lpthread -lconfig_loader -lcommon
OBJECTS=main.o forward_index_connection.o forward_index_server.o
EXECUTABLE=forwardindexserver

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
