CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -fpermissive 
RM = rm -f

.PHONY: all clean test_smart_pointer

all: test_smart_pointer

test_smart_pointer: test_smart_pointer.cpp my_smart_pointer.h
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) test_smart_pointer  *.o 