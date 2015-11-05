#ifndef GRAPH_HH
#define GRAPH_HH

#include "square_symmetric_matrix.hh"
#include <cstddef>
#include <vector>
#include <deque>
#include <stdexcept>

// Generic graph object
template <typename T=double>
class graph {
public:
  typedef std::size_t size_type;
  typedef size_type index_type;
  typedef T value_type;

  // Return the size of the graph (number of nodes)
  virtual size_type size() const = 0;

  // Return the number of neighbors from global node gi
  virtual size_type num_neighbor(index_type gi) const = 0;

  // Return the neighbor global index of neighbor j of global node gi
  virtual index_type neighbor(index_type gi, index_type j) const = 0;

  // Return the weight (possibly directed) from global node gi to neighbor j
  virtual value_type weight(index_type gi, index_type j) const = 0;

  // Return the node weight at global node gi
  virtual value_type node_weight(index_type gi) const = 0;

  // Returns true if the globally indexed nodes are neighbors
  bool is_neighbor(index_type gi, index_type gj) const {
    for (index_type j=0; j<num_neighbor(gi); j++)
      if (neighbor(gi, j) == gj) return true;
    return false;
  }

  // Return the neighbor index of the global index
  index_type neighbor_index(index_type gi, index_type gj) const {
#ifndef NDEBUG
    if (!is_neighbor(gi, gj)) throw std::runtime_error("Not neighbors");
#endif
    index_type index = 0;
    for (index_type j=0; j<num_neighbor(gi); j++)
      if (neighbor(gi, j) == gj) { index = j; break; }
    return index;
  }

  virtual ~graph() { }
};


#endif
