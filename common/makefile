CXXFLAGS=-c -std=c++14 -fPIC $(INCLUDEPATH)
LDFLAGS=-lboost_system -shared
OBJECTS=root_sender.o realtime_sender.o
EXECUTABLE=libcommon.so

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
