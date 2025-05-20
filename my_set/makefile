CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -fpermissive 
RM = rm -f

.PHONY: all clean test_my_set

all: test_my_set

test_my_set: test_my_set.cpp my_set.h
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) test_my_set  *.o 