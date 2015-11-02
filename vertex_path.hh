#ifndef VERTEX_PATH_HH
#define VERTEX_PATH_HH

#include "vertex_set.hh"
#include <deque>

template <typename T>
class vertex_path {
public:
  typedef T value_type;
  typedef vertex_set<value_type> vset_type;
  typedef typename vset_type::size_type size_type;
  typedef std::deque<size_type> container_type;
  typedef typename container_type::iterator iterator;

  vertex_path(const vset_type &v, size_type i) :
    path(), len(0), vs(v) { path.push_back(i); }

  inline size_type size() const { return path.size(); }

  inline value_type length() const { return len; }

  void push_back(size_type i)
  { len += vs.distance(path.back(), i); path.push_back(i); }

  void pop_back()
  {
    size_type a(path.back());
    path.pop_back();
    size_type b(path.back());
    len -= vs.distance(a, b);
  }

  const vset_type& set() const { return vs; }

  size_type& front() { return path.back(); }
  const size_type& front() const { return path.back(); }
  size_type& back() { return path.back(); }
  const size_type& back() const { return path.back(); }

  // expose an iterator
  inline iterator& begin() { return path.begin(); }
  inline iterator& end() { return path.end(); }
  inline iterator& rbegin() { return path.rbegin(); }
  inline iterator& rend() { return path.rend(); }

private:
  container_type path;
  value_type len;
  const vset_type &vs;
};

template <typename T>
bool operator<(const vertex_path<T> &p1, const vertex_path<T> &p2)
{ return p1.length() < p2.length(); }

template <typename T>
bool operator>(const vertex_path<T> &p1, const vertex_path<T> &p2)
{ return p1.length() > p2.length(); }

#endif
