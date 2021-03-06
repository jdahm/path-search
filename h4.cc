#include <omp.h>
#include <iostream>
#include <cmath>
#include <array>
#include <vector>
#include <sstream>

#include "square_symmetric_matrix.hh"
#include "Euclidean_impl.hh"
#include "searchtask_impl.hh"

typedef double real;
typedef unsigned int index_type;
typedef Euclidean_set<real> graph_type;
typedef EH_search_path<real> spath_type;
typedef Euclidean_path<real> gpath_type;

typedef spath_type task_type;
typedef spath_type answer_type;
typedef search_manager<task_type, answer_type> manager_type;


graph_type example_graph() {
  constexpr unsigned int c = 4;
  square_symmetric_matrix<real> dt(c);

  dt.set(0, 1, 2.1);
  dt.set(2, 0, 3.0);
  dt.set(2, 1, 3.2);
  dt.set(3, 0, 8.0);
  dt.set(3, 1, 1.6);
  dt.set(3, 2, 4.9);

#ifndef NDEBUG
  std::cout << dt << std::endl;
#endif

  return graph_type(dt);
}

graph_type create_point_set(index_type c) {
  using std::abs; using std::pow; using std::sin;
  // ( |p1-q1|^(3/2) + |p2-q2|^(3/2) )^(2/3)
  square_symmetric_matrix<real> dt(c);
  std::vector< std::array<real,2> > point(c);

#pragma omp parallel default(shared)
  {
#pragma omp for
    for (unsigned int i=0; i<c; i++) {
      point[i][0] = 100 * sin(i);
      point[i][1] = 101 * cos(i*i);
      // point[i][0] = 1.1 * (i*i   % 17);
      // point[i][1] = 0.5 * (i*i*i % 23);
    }

#pragma omp for collapse(2)
    for (unsigned int p=0; p<c; p++)
      for (unsigned int q=0; q<c; q++)
        dt(p,q) = pow(pow(abs(point[p][0] - point[q][0]), 1.5) +
                      pow(abs(point[p][1] - point[q][1]), 1.5), 2./3.);
  }

#ifndef NDEBUG
  std::cout << dt << std::endl;
#endif

  return graph_type(dt);
}


void find_path_task(task_type &sp, manager_type &manager, const index_type branch_level) {
  // Get current best answer
  answer_type ans = manager.answer();
  do {
    sp.iterate_dfs();
    if (sp > ans) {
      // If the path is longer tan the currently cached bound, skip
      // ahead to the next branch
      sp.next_branch();
    }
    else if (sp.is_bottom()) {
      // If we got here, the answer is better than the currently
      // cached bound. In this case, submit it to the manager (which
      // will ensure it's _actually_ better) and will return the
      // newest best answer.
      ans = manager.conclude(sp);
    }
    else if (sp.global_level() <= branch_level) {
      // Branch off and submit back to the queue
      while (!sp.last_branch())
        manager.give(sp.split());
    }
  } while (!sp.is_top());
}

const answer_type find_path(const graph_type &g, const index_type branch_level) {
  task_type sp(g, 0);
  manager_type manager(sp, longest_path(g));

#pragma omp parallel shared(manager)
  {
    while (!manager.done())
      if (manager.has_work()) {
        task_type sp = manager.get();
        find_path_task(sp, manager, branch_level);
        manager.finish(sp);
      }
  }
  return manager.answer();
}

int main(int argc, char *argv[]) {
  real start_time, end_time;
  index_type c, branch_level;
  std::string prog;
  std::stringstream ss;

  if (argc != 3) throw std::runtime_error("Usage: " + prog + " c branch_level");
  for (int i=0; i<3; i++) ss << argv[i] << ' ';
  ss >> prog >> c >> branch_level;
  std::cout << "Call: " << prog << " " << c << " " << branch_level << std::endl;

  start_time = omp_get_wtime();

  // auto ps = example_graph();
  auto ps = create_point_set(c);
  auto sp = find_path(ps, branch_level);

  end_time = omp_get_wtime();

  std::cout << sp << std::endl;
  std::cout << "Elapsed time: " << end_time - start_time << std::endl;

  return 0;
}
