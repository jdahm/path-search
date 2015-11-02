#ifndef VERTEX_SET_HH
#define VERTEX_SET_HH

#include <vector>

template < typename T, typename Container=std::vector<T> >
class square_symmetric_matrix {
public:
  typedef T value_type;
  typedef Container container_type;
  typedef typename container_type::size_type size_type;

  square_symmetric_matrix() = delete;
  square_symmetric_matrix(size_type m, value_type v=value_type()) :
    n(m), data(m*m, v) { }

  inline size_type size() const { return n; }

  inline value_type& operator()(size_type i, size_type j)
  { return data[i*n + j]; }

  inline const value_type& operator()(size_type i, size_type j) const
  { return data[i*n + j]; }

  inline void set(size_type i, size_type j, value_type dist)
  { square_symmetric_matrix &mat = *this; mat(i,j) = mat(j,i) = dist; }

  inline const value_type& get(size_type i, size_type j) const
  { square_symmetric_matrix &mat = *this; return mat(i,j); }

private:
  size_type n;
  container_type data;
};

template <typename T>
class vertex_set {
public:
  typedef T value_type;
  typedef square_symmetric_matrix<value_type> table_type;
  typedef typename table_type::size_type size_type;
  
  vertex_set(const table_type& t) : table(t) { }

  inline value_type distance(size_type i, size_type j) const
  { return table(i,j); }

  inline size_type size() const { return table.size(); }

private:
  table_type table;
};

#endif
