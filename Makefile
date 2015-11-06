CXX=g++
CXXFLAGS = -std=c++11 -g -O0
# CXXFLAGS = -std=c++11 -g -O3 -march=native -DNDEBUG
CPPFLAGS = -Wall -Wextra -fopenmp

all: h4

DEPS=square_symmetric_matrix.hh graph.hh tree.hh path.hh task.hh Euclidean_impl.hh searchtask_impl.hh
# generic_impl.hh

h4: h4.cc $(DEPS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	rm h4
