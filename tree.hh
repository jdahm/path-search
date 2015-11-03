#ifndef TREE_HH
#define TREE_HH

#include <stdexcept>
#include <set>

class tree {
public:
  typedef unsigned long int size_type;
  typedef size_type index_type;

  // Child number relative to parent
  virtual size_type whoami() const = 0;

  // Level in the tree (0-based)
  virtual size_type level() const = 0;

  // Number of children attached to current node in tree
  virtual size_type num_children() const = 0;

  // Traverse down to child i
  virtual void enqueue(index_type i) = 0;

  // Traverse up to parent
  virtual void dequeue() = 0;

  // Returns true if the tree has a next sibling. This is a rather
  // slow implementation, so tree types implementing this iterface can
  // overwrite it.
  virtual bool has_next_sibling()
  {
    index_type myself = whoami();
    dequeue();
    bool has_next = (num_children() > myself+1);
    enqueue(myself);
    return has_next;
  }

  // Returns true if the tree has no parent
  bool is_top() { return level() == 0; }

  // Returns true if the tree has no children
  bool is_bottom() { return num_children() == 0; }

  // Go to next branch in the tree traversing up, then to next sibling
  void next_branch() {
    while (!has_next_sibling()) {
      if (is_top()) return;
      dequeue();
    }
    next_sibling();
  }

  void iterate_dfs() {
    // Algorithm given here by @aaz:
    // http://stackoverflow.com/questions/5278580/non-recursive-depth-first-search-algorithm
    // std::cout << "Before: " << level() << " " << whoami() << " " << num_children() << " " << has_next_sibling() << std::endl;
    if (num_children() > 0) enqueue(0);
    else next_branch();
    // std::cout << "After: " << level() << " " << whoami() << " " << num_children() << " " << has_next_sibling() << std::endl;
  }

  // Moves current node to next sibling
  void next_sibling() {
#ifndef NDEBUG
    if (!has_next_sibling()) throw std::runtime_error("Next child does not exist");
#endif
    index_type myself = whoami();
    dequeue();
    enqueue(++myself);
  }
};


template <typename T=double>
class search_path : public path<T>, public tree {
public:
  typedef path<T> path_type;
  typedef graph<T> graph_type;
  typedef typename path_type::size_type size_type;
  typedef typename path_type::index_type index_type;
  typedef typename path_type::value_type value_type;
  typedef typename path_type::container_type container_type;
  typedef typename path_type::const_iterator const_iterator;
  typedef typename path_type::const_reverse_iterator const_reverse_iterator;

  search_path(const graph_type &g, index_type gi) :
    rsize(size_type()), local(), p(), sp(),
    total_weight(value_type()), mygraph(g)
  { p.push_back(gi); sp.insert(gi); local.push(0); }

  search_path(const search_path &sp, index_type i) :
    rsize(sp.size()+1), local(), p(sp.p), sp(sp.sp.begin(), sp.sp.end()),
    total_weight(sp.total_weight), mygraph(sp.mygraph)
  { enqueue(i); }

  /* path implementation */

  size_type size() const { return (p.size() - 1) > 0 ? (p.size() - 1) : 0; }

  value_type weight() const { return total_weight; }

  size_type num_neighbor() const { return num_children(); }

  index_type neighbor(index_type i) const {
    typename set_type::const_iterator sit = sp.begin();
    size_type gi = 0, in = 0;
    while (sit != sp.end()) {
      if (gi == *sit) ++sit;
      else in++;
      if (in > i) return gi;
      gi++;
    }
    return gi + (i - in);
  }

  const_iterator begin() const { return p.begin(); }
  const_reverse_iterator rbegin() const { return p.rbegin(); }
  const_iterator end() const { return p.end(); }
  const_reverse_iterator rend() const { return p.rend(); }

  void push_back(index_type i) { enqueue(i); }
  void pop_back() { dequeue(); }

  /* tree implementation */

  size_type whoami() const { return local.top(); }

  size_type level() const { return size() - rsize; }

  size_type num_children() const
  { return (mygraph.size() - global_level() - 1) % mygraph.size(); }

  void enqueue(index_type i) {
#ifndef NDEBUG
    if (i >= num_children()) throw std::runtime_error("Invalid child");
#endif
    total_weight += mygraph.weight(p.back(), i);
    index_type gi = neighbor(i);
    p.push_back(gi);
    sp.insert(gi);
    local.push(i);
    total_weight += mygraph.node_weight(gi);
  }

  void dequeue() {
#ifndef NDEBUG
    if (is_top()) throw std::runtime_error("No parent");
#endif
    index_type j = local.top();
    index_type gi = p.back();
    total_weight -= mygraph.node_weight(gi);
    p.pop_back();
    sp.erase(gi);
    local.pop();
    total_weight -= mygraph.weight(p.back(), j);
  }

  virtual bool has_next_sibling()
  { return whoami() < (num_siblings() - 1); }

private:
  typedef std::stack<index_type> stack_type;
  typedef std::set<size_type> set_type;

  size_type global_level() const { return p.size() - 1; }

  size_type num_siblings() const
  { return (num_children() + 1) % mygraph.size(); }

  size_type rsize;
  stack_type local;
  container_type p;
  set_type sp;
  value_type total_weight;
  const graph_type &mygraph;
};

#endif
