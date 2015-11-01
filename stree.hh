#ifndef STREE_HH
#define STREE_HH

#include "graph.hh"
#include <deque>
#include <set>
#include <stdexcept>

// template <typename T>
struct stree {
  // typedef T value_type;
  typedef double value_type;
  typedef undirected_graph<value_type> graph_type;
  typedef typename std::deque<graph_type::size_type> position_type;
  typedef typename std::set<graph_type::size_type> set_type;
  typedef typename graph_type::size_type size_type;
  // typedef position_type::size_type size_type;

  set_type spos;
  position_type rpos;
  position_type pos;
  size_type me;
  value_type length;
  const graph_type &graph;

  stree() = delete;

  // New stree from root node r
  stree(const graph_type &g, size_type r) :
    spos(), rpos(), pos(), me(-1), length(0), graph(g)
  { pos.push_back(r); spos.insert(r); }

  // New stree from end of another rooted at child i
  stree(const stree& st, size_type i) :
    spos(st.spos), rpos(st.rpos.begin(), st.rpos.end()), pos(),
    me(i), length(st.length), graph(st.graph)
  {
    // insert the active path from st
    rpos.insert(rpos.end(), st.pos.begin(), st.pos.end());

    // go to the child node
    to_child(i);

    // add to the length
    length += parent_length();
  }

  inline size_type num_siblings() const
  { return (graph.num_nodes() - level()) % graph.num_nodes(); }

  inline size_type num_children() const
  { return (graph.num_nodes() - level() - 1) % graph.num_nodes(); }

  inline bool is_root() const { return pos.size() == 1; }
  inline bool is_leaf() const { return num_children() == 0; }

  void to_parent() {
#ifndef NDEBUG
    if (pos.size() == 1) throw std::runtime_error("Invalid parent");
#endif
    length -= parent_length();
    spos.erase(pos.back());
    pos.pop_back();
  }

  void to_child(size_type i) {
#ifndef NDEBUG
    if (i >= num_children()) throw std::runtime_error("Invalid child");
#endif
    pos.push_back(child(i));
    spos.insert(pos.back());

    length += parent_length();
  }

  void to_next_child() {
#ifndef NDEBUG
    if (last_child()) throw std::runtime_error("Invalid child");
#endif
    length -= parent_length();

    int origme = me;
    to_parent();
    to_child(origme+1);

    length += parent_length();
  }

  void to_prev_child() {
#ifndef NDEBUG
    if (me == 0) throw std::runtime_error("Invalid child");
#endif
    length -= parent_length();

    int origme = me;
    to_parent();
    to_child(origme-1);

    length += parent_length();
  }

  size_type child(size_type i) const
  // Specialized method to find global index of child i
  // This assumes:
  // - set_type is sorted by std::less<double>
  // - spos does not have any duplicates (would not make sense for problem)
  {
    set_type::const_iterator sit = spos.begin();
    size_type gi = 0, sin = 0;
    while (sit != spos.end()) {
      if (gi == *sit) ++sit;
      else sin++;
      gi++;
      if (sin > i) return gi;
    }
    return gi + (i - sin);
  }

  inline size_type self() const { return pos.back(); }
  inline size_type parent() const { return pos.rbegin()[1]; }
  inline size_type level() const { return spos.size(); }

  inline value_type parent_length() const
  { return graph.distance(self(), parent()); }

  inline bool last_child() const { return me == num_siblings()-1; }
};

#endif
