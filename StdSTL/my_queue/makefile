CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -fpermissive 
RM = rm -f

.PHONY: all clean test_queue

all: test_queue

test_queue: test_queue.cpp my_queue.h
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) test_queue  *.o 