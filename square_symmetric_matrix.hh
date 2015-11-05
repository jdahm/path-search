#ifndef SQUARE_SYMMETRIC_MATRIX_HH
#define SQUARE_SYMMETRIC_MATRIX_HH

#include <vector>
#include <ostream>

template < typename T, typename Container=std::vector<T> >
class square_symmetric_matrix {
public:
  typedef T value_type;
  typedef Container container_type;
  typedef typename container_type::size_type size_type;
  typedef size_type index_type;

  square_symmetric_matrix() = delete;
  square_symmetric_matrix(size_type m, value_type v=value_type()) :
    n(m), data(m*m, v) { }

  inline size_type size() const { return n; }

  inline value_type& operator()(index_type i, index_type j)
  { return data[i*n + j]; }

  inline const value_type& operator()(index_type i, index_type j) const
  { return data[i*n + j]; }

  inline void set(index_type i, index_type j, value_type dist)
  { square_symmetric_matrix &mat = *this; mat(i,j) = mat(j,i) = dist; }

  inline const value_type& get(index_type i, index_type j) const
  { square_symmetric_matrix &mat = *this; return mat(i,j); }

private:
  size_type n;
  container_type data;
};

template<typename T, typename C>
std::ostream& operator<<(std::ostream& os, const square_symmetric_matrix<T,C> &mat)
{
  typedef typename square_symmetric_matrix<T,C>::index_type index_type;
  for (index_type i=0; i<mat.size(); i++) {
    for (index_type j=0; j<mat.size()-1; j++)
      os << mat(i,j) << "\t";
    os << mat(i,mat.size()-1);
    os << std::endl;
  }
  return os;
}


#endif
