#ifndef SEARCH_HH
#define SEARCH_HH

#include "vertex_set.hh"
#include "vertex_path.hh"

#include <deque>
#include <set>
#include <stdexcept>

class search_tree {
public:
  typedef unsigned int size_type;
  virtual inline size_type whoami() const = 0;
  virtual inline size_type root_size() const = 0;
  virtual inline size_type size() const = 0;
  virtual inline size_type num_children() const = 0;
  virtual inline size_type num_siblings() const = 0;
  virtual void dequeue() = 0;
  virtual void enqueue(size_type i) = 0; // Enqueue child i

  bool is_root() { return size() - root_size() == 0; }
  bool is_leaf() { return num_children() == 0; }
};

template <typename T>
class Hsearch_tree : public search_tree {
public:
  typedef T value_type;
  typedef vertex_set<value_type> vset_type;
  typedef vertex_path<value_type> vpath_type;

  Hsearch_tree(const vset_type &vs, size_type gi) :
    local(), vp(vs, gi), svp(), rsize(0)
  { svp.insert(i); }

  Hsearch_tree(const Hsearch_tree &vs, size_type i) :
    local(vs.local), vp(vs.vp), svp(), rsize(vp.size())
  { svp.insert(svp.end(), vp.begin(), vp.end()); enqueue(i); }

  inline size_type whoami() const { return local.top(); }
  inline size_type root_size() const { return rsize; }
  inline size_type size() const { return vp.size(); }
  inline size_type num_children() const
  { return (vp.set().size() - size()) % vp.set().size(); }
  inline size_type num_siblings() const
  { return (num_children() + 1) % vp.set().size(); }
  // { return (vp.set().size() - size() + 1) % vp.set().size(); }

  void dequeue() {
#ifndef NDEBUG
    if (is_root()) throw std::runtime_error("No parent");
#endif
    size_type i(vp.back());
    vp.pop_back();
    svp.erase(i);
    local.pop();
  }

  void enqueue(size_type i) {
#ifndef NDEBUG
    if (i >= num_children()) throw std::runtime_error("Invalid child");
#endif
    size_type gi(child(i));
    vp.push_back(gi);
    svp.insert(gi);
    local.push(i);
  }

  size_type child(size_type i) const {
    set_type::const_iterator sit = svp.begin();
    size_type gi = 0, in = 0;
    while (sit != svp.end()) {
      if (gi == *sit) ++sit;
      else in++;
      if (in > i) return gi;
      gi++;
    }
    return gi + (i - in);
  }

  const vpath_type& position() const { return vp; }

private:
  typedef typename std::set<size_type> set_type;

  std::stack<size_type> local;
  vpath_type vp;
  set_type svp;
  size_type rsize;
};

path 

#endif
