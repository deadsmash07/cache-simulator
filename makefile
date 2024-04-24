CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic
SRC = cacheSim.cpp   # Add all cpp files here
OBJ = $(SRC:.cpp=.o)  # This line automatically converts SRC (.cpp) files to .o files

all: cacheSim

cacheSim: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) cacheSim
