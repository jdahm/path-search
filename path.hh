#ifndef PATH_HH
#define PATH_HH

#include <deque>
#include <stack>

template <typename T=double>
class path {
public:
  typedef unsigned long int size_type;
  typedef size_type index_type;
  typedef T value_type;
  typedef std::deque<index_type> container_type;
  typedef typename container_type::const_iterator const_iterator;
  typedef typename container_type::const_reverse_iterator const_reverse_iterator;

  // "Size" of the path (number of nodes)
  virtual size_type size() const = 0;

  // "Weight" of the path (e.g. length in the case of a Euclidean path)
  virtual value_type weight() const = 0;

  // Neighbors / options for next step in path
  virtual size_type num_neighbor() const = 0;

  // Neighbors / options for next step in path
  virtual index_type neighbor(index_type i) const = 0;

  // Expose a const iterator
  virtual const_iterator begin() const = 0;
  virtual const_reverse_iterator rbegin() const = 0;
  virtual const_iterator end() const = 0;
  virtual const_reverse_iterator rend() const = 0;

  virtual void push_back(index_type i) = 0;
  virtual void pop_back() = 0;
};


// Comparison operators for paths (compare the weights)
template <typename T>
bool operator<(const path<T> &p1, const path<T> &p2)
{ return p1.weight() < p2.weight(); }

template <typename T>
bool operator>(const path<T> &p1, const path<T> &p2)
{ return p1.weight() > p2.weight(); }


// Traverse a graph
template <typename T=double>
class graph_path : public path<T> {
public:
  typedef path<T> path_type;
  typedef graph<T> graph_type;
  typedef typename path_type::size_type size_type;
  typedef typename path_type::index_type index_type;
  typedef typename path_type::value_type value_type;
  typedef typename path_type::container_type container_type;
  typedef typename path_type::const_iterator const_iterator;
  typedef typename path_type::const_reverse_iterator const_reverse_iterator;

  graph_path(const graph_type &g, index_type gi) :
    local(), p(), total_weight(value_type()), mygraph(g)
  { p.push_back(gi); }

  size_type size() const { return p.size(); }

  value_type weight() const { return total_weight; }

  // pure virtual
  size_type num_neighbor() const
  { return mygraph.num_neighbor(p.back()); }

  // pure virtual
  index_type neighbor(index_type i) const
  { return mygraph.neighbor(p.back(), i); }

  const_iterator begin() const { return p.begin(); }
  const_reverse_iterator rbegin() const { return p.rbegin(); }
  const_iterator end() const { return p.end(); }
  const_reverse_iterator rend() const { return p.rend(); }

  void push_back(index_type i) {
    total_weight += mygraph.weight(p.back(), i);
    p.push_back(neighbor(i));
    local.push(i);
    total_weight += mygraph.node_weight(p.back());
  }

  void pop_back() {
    index_type j = local.top();
    total_weight -= mygraph.node_weight(p.back());
    p.pop_back();
    local.pop();
    total_weight -= mygraph.weight(p.back(), j);
  }

private:
  typedef std::stack<index_type> stack_type;
  stack_type local;
  container_type p;
  value_type total_weight;
  const graph_type &mygraph;
};

#endif
