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
#include <stack>
template <typename T=double>
class EH_search_path : public path<T>, public tree {
public:
  typedef path<T> path_type;
  typedef Euclidean_set<T> graph_type;
  typedef typename path_type::size_type size_type;
  typedef typename path_type::index_type index_type;
  typedef typename path_type::value_type value_type;
  typedef typename path_type::container_type container_type;
  typedef typename path_type::const_iterator const_iterator;
  typedef typename path_type::const_reverse_iterator const_reverse_iterator;

  typedef std::ptrdiff_t difference_type;

  // Forward declaration of Euclidean_path type below this
  // template <typename V=double> class Euclidean_path;
  template <typename V> friend class Euclidean_path;

  template <typename V> friend EH_search_path<V> longest_path(const Euclidean_set<V> &g);

  // Initialize
  EH_search_path(const graph_type &g, index_type gi) :
    rsize(0), local(), p(), sp(),
    total_distance(value_type()), mygraph(g)
  { p.push_back(gi); sp.insert(gi); local.push(0); }

  // Split
  EH_search_path(const EH_search_path &pa, index_type i) :
    rsize(pa.size()+1), local(), p(pa.p), sp(pa.sp.begin(), pa.sp.end()),
    total_distance(pa.total_distance), mygraph(pa.mygraph)
  { enqueue(i); }

  // Copy
  EH_search_path(const EH_search_path &pa) :
    rsize(pa.rsize), local(pa.local), p(pa.p), sp(pa.sp),
    total_distance(pa.total_distance), mygraph(pa.mygraph) { }

  // Create empty
  EH_search_path(const graph_type &g) : p(), total_distance(0), mygraph(g) { }

  // p.size() is always strictly positive, so returning unsigned is OK
  size_type size() const { return (p.size() - 1); }
  value_type weight() const { return total_distance; }

  const_iterator begin() const { return p.begin(); }
  const_reverse_iterator rbegin() const { return p.rbegin(); }
  const_iterator end() const { return p.end(); }
  const_reverse_iterator rend() const { return p.rend(); }

  // size() - rsize is always non-negative so this is safe
  size_type level() const { return size() - rsize; }

  size_type global_level() const { return p.size() - 1; }

  // Split tree
  EH_search_path split() {
    index_type me = whoami();
    dequeue();
    EH_search_path newpath = EH_search_path(*this, me);
    enqueue(me);
    next_branch();
    return newpath;
  }

  EH_search_path& operator=(const EH_search_path &other)
  // This is needed to get around the non-copyable behavior due to the
  // const reference member
  {
    rsize = other.rsize;
    local = other.local;
    p = other.p;
    sp = other.sp;
    total_distance = other.total_distance;
    // mygraph = other.mygraph;
    return *this;
  }

  const graph_type& graph() const { return mygraph; }

private:
  typedef std::stack<index_type> stack_type;
  typedef std::set<size_type> set_type;

  /* path implementation */
  size_type num_neighbor() const { return num_children(); }

  index_type neighbor(index_type ti) const {
    typename set_type::const_iterator sit = sp.begin();
    size_type gi = 0, in = 0;
    while (sit != sp.end()) {
      if (gi == *sit) ++sit;
      else in++;
      if (in > ti) return gi;
      gi++;
    }
    difference_type add = ti - in;
    return gi + add;
  }

  void push_back(index_type ti) { enqueue(ti); }
  void pop_back() { dequeue(); }

  /* tree implementation */
  size_type whoami() const { return local.top(); }

  // The first part is always non-negative so this is safe
  size_type num_children() const
  { return (mygraph.size() - global_level() - 1) % mygraph.size(); }

  void enqueue(index_type i) {
#ifndef NDEBUG
    if (i >= num_children()) throw std::runtime_error("Invalid child");
#endif
    index_type gi = neighbor(i);
    total_distance += mygraph.distance(p.back(), gi);
    p.push_back(gi);
    sp.insert(gi);
    local.push(i);
  }

  void dequeue() {
#ifndef NDEBUG
    if (is_top()) throw std::runtime_error("No parent");
#endif
    index_type gi = p.back();
    p.pop_back();
    sp.erase(gi);
    local.pop();
    total_distance -= mygraph.distance(p.back(), gi);
  }

  virtual bool has_next_sibling() { return whoami() < num_sibling(); }

  size_type num_sibling() const
  { return num_children() % (mygraph.size() - rsize - 1); }

  size_type rsize;
  stack_type local;
  container_type p;
  set_type sp;
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
#include "path.hh"
template <typename T=double>
class Euclidean_path : public path<T> {
public:
  typedef path<T> path_type;
  typedef Euclidean_set<T> graph_type;
  typedef typename path_type::size_type size_type;
  typedef typename path_type::index_type index_type;
  typedef typename path_type::value_type value_type;
  typedef typename path_type::container_type container_type;
  typedef typename path_type::const_iterator const_iterator;
  typedef typename path_type::const_reverse_iterator const_reverse_iterator;

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

private:
  size_type num_neighbor() const
  { return mygraph.num_neighbor(p.back()); }

  index_type neighbor(index_type i) const
  { return mygraph.neighbor(p.back(), i); }

  void push_back(index_type i) {
    index_type gi = neighbor(i);
    total_distance += mygraph.distance(p.back(), gi);
    p.push_back(gi);
  }

  void pop_back() {
    index_type gi = p.back();
    p.pop_back();
    total_distance -= mygraph.distance(p.back(), gi);
  }

  container_type p;
  value_type total_distance;
  const graph_type &mygraph;
};


#endif
