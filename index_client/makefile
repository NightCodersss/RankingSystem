CXX=g++-5
CXXFLAGS=-c -std=c++14 -g
LDFLAGS=-lboost_system -L../UbjsonCpp/src -lcppunit -lUbjsonCpp -lpthread
OBJECTS=main.o
EXECUTABLE=indexclient

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
