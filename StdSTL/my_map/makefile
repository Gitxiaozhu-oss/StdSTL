CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -fpermissive 
RM = rm -f

.PHONY: all clean test_map

all: test_map

test_map: test_map.cpp my_map.h
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) test_map  *.o 