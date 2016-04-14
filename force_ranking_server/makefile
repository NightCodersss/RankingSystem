CXXFLAGS=-g -DBOOST_LOG_DYN_LINK -c -std=c++14 $(INCLUDEPATH)
LDFLAGS=-lboost_log_setup -lboost_log -lcommon -lboost_system -lboost_thread $(LIBRARYPATH) -lUbjsonCpp -lpthread -lconfig_loader
OBJECTS=main.o force_ranking_connection.o force_ranking_server.o
EXECUTABLE=forcerankingserver

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
