#CXX=g++-5
CXXFLAGS=-c -DBOOST_LOG_DYN_LINK -g -std=c++14 $(INCLUDEPATH)
LDFLAGS=-lboost_log_setup -lcommon -lboost_log -lboost_system -lboost_thread $(LIBRARYPATH) -lUbjsonCpp -lpthread -lconfig_loader -lboost_timer
OBJECTS=main.o ranking_connection.o ranking_server.o ranking_struct.o south_request.o north_request.o streams_dispatcher.o
EXECUTABLE=rankingserver

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) log.log
