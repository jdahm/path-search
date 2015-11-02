#include <omp.h>
#include <iostream>
#include <cmath>
#include <array>
#include <vector>
#include "vertex_set.hh"
#include "vertex_path.hh"
#include "search_tree.hh"

template<typename T>
vertex_set<T> example_graph() {
  constexpr unsigned int c = 4;
  square_symmetric_matrix<T> dt(c);

  dt.set(0, 1, 2.1);
  dt.set(2, 0, 3.0);
  dt.set(2, 1, 3.2);
  dt.set(3, 0, 8.0);
  dt.set(3, 1, 1.6);
  dt.set(3, 2, 4.9);

  return vertex_set<T>(dt);
}

template<typename T>
vertex_set<T> create_vertex_set() {
  using std::abs; using std::pow;
  // ( |p1-q1|^(3/2) + |p2-q2|^(3/2) )^(2/3)
  constexpr unsigned int c = 8;
  square_symmetric_matrix<T> dt(c);
  std::vector< std::array<T,2> > point(c);

#pragma omp parallel default(shared)
  {
#pragma omp for
    for (unsigned int i=0; i<c; i++) {
      point[i][0] = 1.1 * (i*i   % 17);
      point[i][1] = 0.5 * (i*i*i % 23);
    }

#pragma omp for collapse(2)
    for (unsigned int p=0; p<c; p++)
      for (unsigned int q=0; q<c; q++) {
        dt(p,q) = pow(pow(abs(point[p][0] - point[q][0]), 1.5) +
                      pow(abs(point[p][1] - point[q][1]), 1.5), 2./3.);
      }
  }

  return vertex_set<T>(dt);
}

int main() {

  auto vs(create_vertex_set<double>());

  auto vp1(vertex_path<double>(vs, 0));
  auto vp2 = vp1;

  // rgraph *g = create_graph();

  // stree *t = create_stree(g, 0);

  // for (int p=0; p<g.num_nodes(); p++) {
  //   for (int q=0; q<p; q++) {
  //     std::cout << g.distance(p,q) << "\t";
  //   }
  //   std::cout << std::endl;
  // }

  return 0;
}
