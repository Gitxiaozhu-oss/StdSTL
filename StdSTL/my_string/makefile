CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -fpermissive 
RM = rm -f

.PHONY: all clean test_string

all: test_string

test_string: test_string.cpp my_string.h
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) test_string  *.o 