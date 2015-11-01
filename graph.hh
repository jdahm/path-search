#ifndef GRAPH_HH
#define GRAPH_HH

#include <vector>

template <typename T>
class symmetric_matrix {
public:
  typedef T data_type;
  typedef std::vector<T> container_type;
  typedef typename container_type::size_type size_type;

  symmetric_matrix(size_type size, data_type fill=data_type()) :
    data(size*size, fill), n(size) { }

  inline T& operator()(size_type i, size_type j)
  { return data[i*n+j]; }
  inline const T& operator()(size_type i, size_type j) const
  { return data[i*n+j]; }

  inline void set(size_type i, size_type j, data_type dist) {
    symmetric_matrix &table = *this;
    table(i,j) = table(j,i) = dist;
  }

  inline const T& get(size_type i, size_type j) const {
    symmetric_matrix &table = *this;
    return table(i,j);
  }

  inline const size_type size() const { return n; }

private:
  std::vector<data_type> data;
  size_type n;
};

template <typename T>
class undirected_graph {
public:
  typedef T dist_type;
  typedef symmetric_matrix<dist_type> table_type;
  typedef typename table_type::size_type size_type;

  undirected_graph(const table_type& t) : table(t) { }

  inline dist_type distance(size_type i, size_type j) const
  { return table(i,j); }

  inline const size_type num_nodes() const { return table.size(); }

private:
  table_type table;
};

#endif
