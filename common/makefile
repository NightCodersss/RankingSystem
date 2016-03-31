CXXFLAGS=-c -DBOOST_LOG_DYN_LINK -g -std=c++14 -fPIC $(INCLUDEPATH)
LDFLAGS=-lboost_log -lboost_thread -lboost_system -shared
OBJECTS=batch_sender.o realtime_sender.o sender.o fetcher.o document.o query.o query_tree.o query_parser.o query_parse_error.o
EXECUTABLE=libcommon.so

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
