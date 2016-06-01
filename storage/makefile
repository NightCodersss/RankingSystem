CXX=clang++
CXXFLAGS=-c -DBOOST_LOG_DYN_LINK -g -std=c++14 -fPIC $(INCLUDEPATH)
LDFLAGS=-lboost_log -lboost_thread -lboost_system -shared -lboost_filesystem
OBJECTS=serialize.o block.o filestream.o bigstorage.o bigtest.o
EXECUTABLE=libstorage.so

all: $(EXECUTABLE) bigtest

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

bigtest: $(OBJECTS)
	$(CXX) -g -std=c++14 $(OBJECTS) -lboost_system -lboost_filesystem -o bigtest

clean_index:
	rm -rf index/*
