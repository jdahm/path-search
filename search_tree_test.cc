#include <iostream>
#include <stdexcept>

#include "search_tree.hh"

vertex_set<double> example_graph() {
  constexpr unsigned int c = 4;
  square_symmetric_matrix<double> dt(c);

  dt.set(0, 1, 2.1);
  dt.set(2, 0, 3.0);
  dt.set(2, 1, 3.2);
  dt.set(3, 0, 8.0);
  dt.set(3, 1, 1.6);
  dt.set(3, 2, 4.9);

  return square_symmetric_matrix<double>(dt);
}

int main() {
  auto vs(example_graph());
  // build an stree
  Hsearch_tree<double> st(vs, 0);

  if (st.num_children() != vs.size()-1) {
    std::cerr << "num_children != num_nodes - 1" << std::endl;
    return 1;
  }

  // std::cout << st.num_children() << " : " << st.child(0) << " " << st.child(1) << " " << st.child(2) << std::endl;
  st.enqueue(0);

  if (st.my_index() != 1) {
    std::cerr << "my_index != 1" << std::endl;
    return 1;
  }

  if (st.num_children() != 2) {
    std::cerr << "num_children != 2" << std::endl;
    return 1;
  }

  st.enqueue(0);

  if (st.child(0) != 3) {
    std::cerr << "child(0) != 3" << std::endl;
    return 1;
  }

  st.enqueue(0);

  if (!st.is_leaf()) {
    std::cerr << "is_leaf() failed" << std::endl;
    return 1;
  }

  st.dequeue(); st.dequeue();

  if (st.child(1) != 3) {
    std::cerr << "child(0) != 3" << std::endl;
    return 1;
  }

  st.enqueue(1);

  if (st.child(0) != 2) {
    std::cerr << "child(0) != 2" << std::endl;
    return 1;
  }

  return 0;
}
