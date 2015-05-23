CXX=g++-5
CXXFLAGS=-c -std=c++14 -fPIC
LDFLAGS=-lboost_system -shared
OBJECTS=config_loader.o
EXECUTABLE=config_loader.so

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
