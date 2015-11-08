#ifndef TREE_HH
#define TREE_HH

#include "path.hh"
#include <cstddef>
#include <set>
#include <stdexcept>

class tree {
public:
  typedef std::size_t size_type;
  typedef size_type index_type;

  // Level in the tree (0-based)
  virtual size_type level() const = 0;

  // Returns true if the tree has no parent
  bool is_top() { return level() == 0; }

  // Returns true if the tree has no children
  bool is_bottom() { return num_children() == 0; }

  // Returns true if we're on the last branch at this level
  bool last_branch() { return !has_next_sibling(); }

  // Go to next branch in the tree traversing up, then to next sibling
  void next_branch() {
    while (last_branch()) {
      if (is_top()) return;
      dequeue();
    }
    next_sibling();
  }

  // Take a step of dfs. If at the bottom with no neighbors, traverse up and onto next branch
  void iterate_dfs() {
    if (num_children() > 0) enqueue(0);
    else next_branch();
  }

  virtual ~tree() { }
private:
  // Child number relative to parent
  virtual size_type whoami() const = 0;

  // Number of children attached to current node in tree
  virtual size_type num_children() const = 0;

  // Traverse down to child i
  virtual void enqueue(index_type i) = 0;

  // Traverse up to parent
  virtual void dequeue() = 0;

  // Returns true if the tree has a next sibling. This is a rather
  // slow implementation, so tree types implementing this iterface can
  // overwrite it.
  virtual bool has_next_sibling() {
    if (is_top()) return false;
    index_type myself = whoami();
    dequeue();
    bool has_next = (num_children() > myself+1);
    enqueue(myself);
    return has_next;
  }

  // Moves current node to next sibling
  void next_sibling() {
    // It is the caller's responsibility to ensure that there actually is a next sibling
#ifndef NDEBUG
    if (!has_next_sibling()) throw std::runtime_error("Next child does not exist");
#endif
    // if (!has_next_sibling()) return;
    index_type next = whoami() + 1;
    dequeue();
    enqueue(next);
  }

};

#endif
