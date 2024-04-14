CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic

all: cacheSim

cacheSim: cacheSim.cpp
	$(CXX) $(CXXFLAGS) -o cacheSim cacheSim.cpp

clean:
	rm -f *.o cacheSim
