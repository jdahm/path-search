#ifndef GENERIC_IMPL_HH
#define GENERIC_IMPL_HH

#include "graph.hh"
#include "path.hh"
#include "tree.hh"

// Hamiltonian search path through a graph
template <typename T=double>
class Hamiltonian_search_path : public path<T>, public tree {
public:
  typedef path<T> path_type;
  typedef graph<T> graph_type;
  typedef typename path_type::size_type size_type;
  typedef typename path_type::index_type index_type;
  typedef typename path_type::value_type value_type;
  typedef typename path_type::container_type container_type;
  typedef typename path_type::const_iterator const_iterator;
  typedef typename path_type::const_reverse_iterator const_reverse_iterator;

  typedef std::ptrdiff_t difference_type;

  // Initialize
  Hamiltonian_search_path(const graph_type &g, index_type gi) :
    rsize(0), local(), p(), sp(),
    total_weight(value_type()), mygraph(g)
  { p.push_back(gi); sp.insert(gi); local.push(0); total_weight += mygraph.node_weight(gi); }

  // Split
  Hamiltonian_search_path(const Hamiltonian_search_path &sp, index_type i) :
    rsize(sp.size()+1), local(), p(sp.p), sp(sp.sp.begin(), sp.sp.end()),
    total_weight(sp.total_weight), mygraph(sp.mygraph)
  { enqueue(i); }

  // Copy
  Hamiltonian_search_path(const Hamiltonian_search_path &sp) :
    rsize(sp.rsize), local(sp.local), p(sp.p), sp(sp.sp),
    total_weight(sp.total_weight), mygraph(sp.mygraph) { }

  // p.size() is always strictly positive, so it's safe to return an unsigned size_type here
  size_type size() const { return (p.size() - 1); }
  value_type weight() const { return total_weight; }

  const_iterator begin() const { return p.begin(); }
  const_reverse_iterator rbegin() const { return p.rbegin(); }
  const_iterator end() const { return p.end(); }
  const_reverse_iterator rend() const { return p.rend(); }

  const graph_type& base_graph() const { return mygraph; }

  Hamiltonian_search_path& operator=(const Hamiltonian_search_path &other) {
    rsize = other.rsize;
    local = other.local;
    p = other.p;
    sp = other.sp;
    total_weight = other.total_weight;
    // mygraph = other.base_graph();
    return *this;
  }

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

  // size() - rsize is always non-negative so this is safe
  size_type level() const { return size() - rsize; }

  // The first part is always non-negative so this is safe
  size_type num_children() const
  { return (mygraph.size() - global_level() - 1) % mygraph.size(); }

  void enqueue(index_type ti) {
#ifndef NDEBUG
    if (ti >= num_children()) throw std::runtime_error("Invalid child");
#endif
    index_type gi = neighbor(ti);
    index_type i = mygraph.neighbor_index(p.back(), gi);
    total_weight += mygraph.weight(p.back(), i);
    p.push_back(gi);
    sp.insert(gi);
    local.push(ti);
    total_weight += mygraph.node_weight(gi);
  }

  void dequeue() {
#ifndef NDEBUG
    if (is_top()) throw std::runtime_error("No parent");
#endif
    index_type tj = local.top();
    index_type gi = p.back();
    total_weight -= mygraph.node_weight(gi);
    p.pop_back();
    sp.erase(gi);
    local.pop();
    index_type j = mygraph.neighbor_index(p.back(), gi);
    total_weight -= mygraph.weight(p.back(), j);
  }

  virtual bool has_next_sibling() { return whoami() < num_sibling(); }

  size_type global_level() const { return p.size() - 1; }

  size_type num_sibling() const
  { return num_children() % (mygraph.size() - rsize - 1); }

  size_type rsize;
  stack_type local;
  container_type p;
  set_type sp;
  value_type total_weight;
  const graph_type &mygraph;
};


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
  { p.push_back(gi); total_weight += mygraph.node_weight(gi); }

  graph_path(const Hamiltonian_search_path<T> &sp) :
    local(), p(), total_weight(), mygraph(sp.base_graph()) {
    typedef typename Hamiltonian_search_path<T>::const_iterator spath_iterator;
    // Start the path
    p.push_back(*sp.begin()); 
    total_weight += mygraph.node_weight(p.back());
    // Add the rest of the path
    for (spath_iterator it=sp.begin(); it!=sp.end()-1; ++it)
      push_back(mygraph.neighbor_index(*it, *std::next(it)));
  }

  size_type size() const { return p.size(); }
  value_type weight() const { return total_weight; }

  const_iterator begin() const { return p.begin(); }
  const_reverse_iterator rbegin() const { return p.rbegin(); }
  const_iterator end() const { return p.end(); }
  const_reverse_iterator rend() const { return p.rend(); }

private:
  typedef std::stack<index_type> stack_type;

  size_type num_neighbor() const
  { return mygraph.num_neighbor(p.back()); }

  index_type neighbor(index_type i) const
  { return mygraph.neighbor(p.back(), i); }

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

  stack_type local;
  container_type p;
  value_type total_weight;
  const graph_type &mygraph;
};


#endif
