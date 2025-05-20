CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
RM = rm -f

.PHONY: all clean list

all: list_test

list_test: list_test.cpp my_list.h
	$(CXX) $(CXXFLAGS) -o $@ $<

list: list_test
	./list_test

clean:
	$(RM) list_test  *.o 