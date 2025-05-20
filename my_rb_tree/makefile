CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
RM = rm -f

.PHONY: all clean test_rb_tree test_rb_tree_perf

all: test_rb_tree test_rb_tree_perf

test_rb_tree: test_rb_tree.cpp my_rb_tree.h
	$(CXX) $(CXXFLAGS) -o $@ $<

test_rb_tree_perf: test_rb_tree_perf.cpp my_rb_tree.h
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) test_rb_tree test_rb_tree_perf vector_test *.o 