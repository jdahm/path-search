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
    rsize(0), tlevel(0), p(g.size()),
    total_distance(value_type()), mygraph(g) {
    init_path();
    for (size_type j=gi; j>0; j--) p[j] = p[j-1];
    p[0] = gi;
  }

  // Create empty
  EH_search_path(const graph_type &g) :
    rsize(0), tlevel(0), p(g.size()),
    total_distance(0), mygraph(g) { init_path(); }

  // Split
  EH_search_path(const EH_search_path &pa, index_type i) :
    rsize(pa.global_level()), tlevel(0), p(pa.p),
    total_distance(pa.total_distance), mygraph(pa.mygraph)
  { enqueue(i); }

  // Copy
  EH_search_path(const EH_search_path &pa) :
    rsize(pa.rsize), tlevel(pa.tlevel), p(pa.p),
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

  // Doesn't work?
  size_type global_level() const { return rsize + tlevel; }

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
    tlevel = other.tlevel;
    p = other.p;
    total_distance = other.total_distance;
    // mygraph = other.mygraph;
    return *this;
  }

  void push_back(index_type ti) { enqueue(ti); }
  void pop_back() { dequeue(); }

  const graph_type& graph() const { return mygraph; }

private:
  void init_path() {
    std::iota(p.begin(), p.end(), 0);
    // index_type n = { 0 };
    // std::generate(p.begin(), p.end(), [&n]{ return n++; });
  }

  /* path implementation */
  size_type num_neighbor() const { return num_children(); }

  index_type neighbor(index_type ti) const { return p[global_level() + 1 + ti]; }

  /* tree implementation */
  size_type whoami() const {
    const int gl = global_level();
    const_iterator pit = p.begin() + gl + 1;
    const index_type pt = p[gl];
    const_iterator nit = std::find_if(pit, p.end(),
                                      [&] (const index_type &a) -> bool { return pt < a; });
    return std::distance(pit, nit);
  }

  // The first part is always non-negative so this is safe
  size_type num_children() const
  { return (mygraph.size() - global_level() - 1) % mygraph.size(); }

  void enqueue(index_type i) {
#ifndef NDEBUG
    if (i >= num_children()) throw std::runtime_error("Invalid child");
#endif
    // std::cout << "begin enqueuing: to neighbor=" << i << "=" << neighbor(i) << " " << global_level() << " " << *this << " , " << whoami() << std::endl;
    const size_type gl = global_level();
    const index_type gi = neighbor(i);
    // std::cout << "weight before: " << total_distance << " " << gl << " " << gi << std::endl;
    for (size_type j=gl+1+i; j>gl+1; j--) p[j] = p[j-1];
    p[gl+1] = gi;
    total_distance += mygraph.distance(p[gl], p[gl+1]);
    tlevel++;
    // std::cout << "weight after: " << total_distance << std::endl;
    // std::cout << "end enqueuing: " << global_level() << " " << *this << " , " << whoami() << std::endl;
  }

  void dequeue() {
#ifndef NDEBUG
    if (is_top()) throw std::runtime_error("No parent");
#endif
    index_type i = whoami();
    // std::cout << "begin dequeuing to neighbor=" << i << "=" << neighbor(i) << " " << global_level() << " " << *this << " , " << whoami() << std::endl;
    tlevel--;
    const size_type gl = global_level();
    const index_type gi = p[gl+1];
    total_distance -= mygraph.distance(p[gl], p[gl+1]);
    for (size_type j=gl+1; j<gl+1+i; j++) p[j] = p[j+1];
    p[gl+1+i] = gi;
    // std::cout << "end dequeuing: " << global_level() << " " << *this << " , " << whoami() << std::endl;
  }

  virtual bool has_next_sibling() { return whoami() < num_sibling(); }

  size_type num_sibling() const
  { return num_children() % (mygraph.size() - rsize - 1); }

  size_type rsize, tlevel;
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
