#ifndef GRAPH_HH
#define GRAPH_HH

#include "square_symmetric_matrix.hh"
#include <stdexcept>
#include <vector>
#include <deque>

// Generic graph object
template <typename T=double>
class graph {
public:
  typedef unsigned long int size_type;
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
  index_type local_neighbor(index_type gi, index_type gj) const {
#ifndef NDEBUG
    if (!is_neighbor(gi, gj)) throw std::runtime_error("Not neighbors");
#endif
    for (index_type j=0; j<num_neighbor(gi); j++)
      if (neighbor(gi, j) == gj) return j;
  }
};

// Euclidean point set graph
template <typename T=double>
class Euclidean_point_set : public graph<T> {
public:
  typedef graph<T> graph_type;
  typedef square_symmetric_matrix<T> table_type;
  typedef typename graph_type::size_type size_type;
  typedef typename graph_type::index_type index_type;
  typedef typename graph_type::value_type value_type;

  Euclidean_point_set(const table_type& t) : table(t) { }

  size_type size() const
  { return table.size(); }

  size_type num_neighbor(index_type gi) const
  { (void)gi; return size() - 1; }

  size_type neighbor(index_type gi, index_type j) const
  { return (j < gi) ? j : j + 1; }

  value_type weight(index_type gi, index_type j) const
  { return table(gi, neighbor(gi,j)); }

  value_type node_weight(index_type gi) const
  { return table(gi, gi); }

  // Returns the distance between global nodes i and j
  value_type distance(index_type i, index_type j) const
  { return table(i, j); }

private:
  table_type table;
};


#endif
