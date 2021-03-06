#ifndef EUCLIDEAN_IMPL_HH
#define EUCLIDEAN_IMPL_HH

// Euclidean point set (all-connected graph implied)
#include "square_symmetric_matrix.hh"
#include "graph.hh"
template <typename T=double>
class Euclidean_set : public graph<T> {
public:
  typedef graph<T> graph_type;
  typedef square_symmetric_matrix<T> table_type;
  typedef typename graph_type::size_type size_type;
  typedef typename graph_type::index_type index_type;
  typedef typename graph_type::value_type value_type;

  Euclidean_set(const table_type& t) : table(t) { }

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

// Hamiltonian search path through a graph -- implied tree
#include "path.hh"
#include "tree.hh"
#include <vector>
#include <stack>
#include <algorithm>
template <typename T=double>
class EH_search_path : public path< T, std::vector<std::size_t> >, public tree {
public:
  typedef path< T, std::vector<std::size_t> > base_type;
  typedef Euclidean_set<T> graph_type;
  typedef typename base_type::size_type size_type;
  typedef typename base_type::index_type index_type;
  typedef typename base_type::value_type value_type;
  typedef typename base_type::container_type container_type;
  typedef typename base_type::const_iterator const_iterator;
  typedef typename base_type::const_reverse_iterator const_reverse_iterator;

  typedef std::ptrdiff_t difference_type;

  // Forward declaration of Euclidean_path type below this
  // template <typename V=double> class Euclidean_path;
  template <typename V> friend class Euclidean_path;

  template <typename V> friend EH_search_path<V> longest_path(const Euclidean_set<V> &g);

  // Initialize
  EH_search_path(const graph_type &g, index_type gi) :
    rsize(0), tlevel(0), local(), p(g.size()),
    total_distance(value_type()), mygraph(g) {
    init_path(); init_local();
    std::copy(p.begin(), p.begin()+gi, p.begin()+1);
    p[0] = gi;
  }

  // Create empty
  EH_search_path(const graph_type &g) :
    rsize(0), tlevel(0), local(), p(g.size()),
    total_distance(0), mygraph(g) { init_path(); init_local(); }

  // Copy
  EH_search_path(const EH_search_path &pa) :
    rsize(pa.rsize), tlevel(pa.tlevel), local(pa.local), p(pa.p),
    total_distance(pa.total_distance), mygraph(pa.mygraph) { }

  // p.size() is always strictly positive, so returning unsigned is OK
  size_type size() const { return global_level() + 1; }
  value_type weight() const { return total_distance; }

  const_iterator begin() const { return p.begin(); }
  const_reverse_iterator rbegin() const { return p.rbegin(); }
  const_iterator end() const { return p.begin() + global_level() + 1; }
  // const_iterator end() const { return p.end(); }
  const_reverse_iterator rend() const { return p.rend(); }

  // size() - rsize is always non-negative so this is safe
  size_type level() const { return tlevel; }

  size_type global_level() const { return rsize + tlevel; }

  // Split tree
  EH_search_path split() {
    EH_search_path nsp(mygraph);
    nsp.rsize = global_level();
    nsp.p = p;
    nsp.local.push(whoami());
    nsp.total_distance = total_distance;
    next_branch();
    return nsp;
  }

  // This is needed to get around the non-copyable behavior due to the
  // const reference member
  EH_search_path& operator=(const EH_search_path &other) {
    rsize = other.rsize;
    tlevel = other.tlevel;
    p = other.p;
    local = other.local;
    total_distance = other.total_distance;
    // mygraph = other.mygraph;
    return *this;
  }

  void push_back(index_type ti) { enqueue(ti); }
  void pop_back() { dequeue(); }

  const graph_type& graph() const { return mygraph; }

private:
  typedef std::stack<index_type> stack_type;

  void init_path() { std::iota(p.begin(), p.end(), 0); }
  void init_local() { local.push(0); }

  /* path implementation */
  size_type num_neighbor() const { return num_children(); }

  index_type neighbor(index_type ti) const { return p[global_level() + 1 + ti]; }

  /* tree implementation */
  size_type whoami() const { return local.top(); }

  // The first part is always non-negative so this is safe
  size_type num_children() const
  { return (mygraph.size() - global_level() - 1) % mygraph.size(); }

  void enqueue(index_type i) {
#ifndef NDEBUG
    if (i >= num_children()) throw std::runtime_error("Invalid child");
#endif
    const size_type gl = global_level();
    const index_type gi = neighbor(i);
    std::copy(p.begin()+gl+1, p.begin()+gl+1+i, p.begin()+gl+2);
    p[gl+1] = gi;
    total_distance += mygraph.distance(p[gl], p[gl+1]);
    tlevel++;
    local.push(i);
  }

  void dequeue() {
#ifndef NDEBUG
    if (is_top()) throw std::runtime_error("No parent");
#endif
    const index_type i = whoami();
    const size_type gl = global_level();
    const index_type gi = p[gl];
    total_distance -= mygraph.distance(p[gl-1], p[gl]);
    std::copy_backward(p.begin()+gl+1, p.begin()+gl+1+i, p.begin()+gl+i);
    p[gl+i] = gi;
    tlevel--;
    local.pop();
  }

  bool has_next_sibling() { return whoami() < num_sibling(); }

  size_type num_sibling() const
  { return num_children() % (mygraph.size() - rsize - 1); }

  index_type rsize, tlevel;
  stack_type local;
  container_type p;
  value_type total_distance;
  const graph_type &mygraph;
};


#include <limits>
template <typename V>
EH_search_path<V> longest_path(const Euclidean_set<V> &g) {
  EH_search_path<V> sp(g);
  sp.total_distance = std::numeric_limits<typename EH_search_path<V>::value_type>::max();
  return sp;
}


// Traverse a (Euclidean) graph -- really just an ordered set of points
// and weights
// This path implementation is not used
template <typename T=double>
class Euclidean_path : public path< T, std::deque<std::size_t> > {
public:
  typedef path< T, std::deque<std::size_t> > base_type;
  typedef Euclidean_set<T> graph_type;
  typedef typename base_type::size_type size_type;
  typedef typename base_type::index_type index_type;
  typedef typename base_type::value_type value_type;
  typedef typename base_type::container_type container_type;
  typedef typename base_type::const_iterator const_iterator;
  typedef typename base_type::const_reverse_iterator const_reverse_iterator;

  Euclidean_path(const graph_type &g, index_type gi) :
    p(), total_distance(value_type()), mygraph(g)
  { p.push_back(gi); total_distance += mygraph.node_weight(gi); }

  Euclidean_path(const EH_search_path<T> &sp) :
    p(sp.p), total_distance(sp.total_distance), mygraph(sp.mygraph) { }

  size_type size() const { return p.size(); }
  value_type weight() const { return total_distance; }

  const_iterator begin() const { return p.begin(); }
  const_reverse_iterator rbegin() const { return p.rbegin(); }
  const_iterator end() const { return p.end(); }
  const_reverse_iterator rend() const { return p.rend(); }

  Euclidean_path& operator=(const Euclidean_path &other)
  // This is needed to get around the non-copyable behavior due to the
  // const reference member
  { p = other.p; total_distance = other.total_distance; return *this; }

  void push_back(index_type gi) {
    total_distance += mygraph.distance(p.back(), gi);
    p.push_back(gi);
  }

  void pop_back() {
    index_type gi = p.back();
    p.pop_back();
    total_distance -= mygraph.distance(p.back(), gi);
  }

private:
  size_type num_neighbor() const
  { return mygraph.num_neighbor(p.back()); }

  index_type neighbor(index_type i) const
  { return mygraph.neighbor(p.back(), i); }

  container_type p;
  value_type total_distance;
  const graph_type &mygraph;
};


#endif
