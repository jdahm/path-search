#ifndef PATH_HH
#define PATH_HH

#include <cstddef>
#include <iostream>
#include <deque>

template < typename T=double, typename Container=std::deque<std::size_t> >
class path {
public:
  typedef std::size_t size_type;
  typedef size_type index_type;
  typedef T value_type;
  typedef Container container_type;
  typedef typename container_type::const_iterator const_iterator;
  typedef typename container_type::const_reverse_iterator const_reverse_iterator;

  // "Size" of the path (number of nodes)
  virtual size_type size() const = 0;

  // "Weight" of the path (e.g. length in the case of a Euclidean path)
  virtual value_type weight() const = 0;

  // Expose a const iterator
  virtual const_iterator begin() const = 0;
  virtual const_reverse_iterator rbegin() const = 0;
  virtual const_iterator end() const = 0;
  virtual const_reverse_iterator rend() const = 0;

  virtual void push_back(index_type i) = 0;
  virtual void pop_back() = 0;

  virtual ~path() { }
private:
  // Neighbors / options for next step in path
  virtual size_type num_neighbor() const = 0;

  // Neighbors / options for next step in path
  virtual index_type neighbor(index_type i) const = 0;
};

// Comparison operators for paths (compare the weights)
template <typename T, typename C1, typename C2>
bool operator<(const path<T,C1> &p1, const path<T,C2> &p2)
{ return p1.weight() < p2.weight(); }

template <typename T, typename C1, typename C2>
bool operator>(const path<T,C1> &p1, const path<T,C2> &p2)
{ return p1.weight() > p2.weight(); }

// Print path
template <typename V, typename C>
std::ostream& operator<<(std::ostream& os, const path<V,C> &p) {
  typedef typename path<V,C>::const_iterator path_iterator;
  os << "path: ";
  for (path_iterator it=p.begin(); it != p.end(); ++it)
    os << (*it) << " ";
  os << "weight: " << p.weight();
  return os;
}


#endif
