#include "graph.hh"
#include "stree.hh"

#include <iostream>
#include <stdexcept>

typedef double real;
typedef undirected_graph<real> rgraph;

rgraph example_graph() {
  constexpr unsigned int c = 4;
  symmetric_matrix<real> dt(c);

  dt.set(0, 1, 2.1);
  dt.set(2, 0, 3.0);
  dt.set(2, 1, 3.2);
  dt.set(3, 0, 8.0);
  dt.set(3, 1, 1.6);
  dt.set(3, 2, 4.9);

  return rgraph(dt);
}

int main() {
  rgraph graph(example_graph());

  // build an stree
  stree st(graph, 0);

  if (st.num_children() != graph.num_nodes()-1) {
    std::cerr << "num_children != num_nodes - 1" << std::endl;
    return 1;
  }

  // check children
  if (st.child(0) != 1) {
    std::cerr << "1: child(1) != 1" << std::endl;
    return 1;    
  }

  // if (st.self())

  return 0;
}
