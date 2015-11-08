#ifndef SEARCHTASK_IMPL_HH
#define SEARCHTASK_IMPL_HH

#include <omp.h>
#include <queue>
#include <functional>

// Search task queue -- any call to this must 
#include "task.hh"
template <typename T>
class search_queue : public task_queue<T> {
public:
  // typedef std::queue<T> container_type;
  typedef std::stack<T> container_type;
  typedef task_queue<T> base_type;
  typedef typename base_type::task_type task_type;
  typedef typename base_type::size_type size_type;

  template <typename V, typename A> friend class search_manager;

  search_queue() : container() { omp_init_lock(&lock); }

  void add(const task_type &t)
  { get_lock(); container.push(t); release_lock(); }

  task_type get() {
    get_lock();
    // task_type task = container.front();
    task_type task = container.top();
    container.pop();
    release_lock();
    return task;
  }

  size_type size() { return container.size(); }

  ~search_queue() { omp_destroy_lock(&lock); }

private:
  void get_lock() { omp_set_lock(&lock); }
  void release_lock() { omp_unset_lock(&lock); }

  omp_lock_t lock;
  container_type container;
};


template <typename T, typename A>
class search_manager : public task_manager<T,A> {
public:
  typedef task_manager<T,A> base_type;
  typedef search_queue<T> tqueue_type;
  typedef typename base_type::task_type task_type;
  typedef typename base_type::answer_type answer_type;
  typedef typename base_type::size_type size_type;

  template <typename V, typename W> friend
  std::ostream& operator<<(std::ostream &os, search_manager<V,W> &manager);

  search_manager(const task_type &first_task, const answer_type& initial_answer) :
    queue(), ntask(0), ans(initial_answer), alloc(false)
  { omp_init_lock(&lock); give(first_task); }

  task_type get() {
#ifndef NDEBUG
    if (!alloc) throw std::runtime_error("get(): alloc was not set");
#endif
    // The order of the following statements matters
    task_type task = queue.get();
    alloc = false;
    return task;
  }

  void give(const task_type &task) {
    queue.add(task);
#pragma omp atomic update
    ntask++;
  }

  void finish(const task_type &task) {
    (void)task; // task is unused here
#pragma omp atomic update
    ntask--;
  }

  answer_type& conclude(const answer_type &a) {
    if (a < ans) {
      get_lock();
      ans = a;
      release_lock();
    }
    return ans;
  }

  bool done() const {
    size_type nt;
    // Seems to not be necessary here
// #pragma omp atomic read
    nt = ntask;
    return (ntask == 0);
  }

  bool has_work() {
    bool work;
    get_lock();
    if (!alloc) {
      work = (queue.size() > 0);
      if (work) alloc = true;
    }
    else work = false;
    release_lock();
    return work;
  }

  const answer_type& answer() const { return ans; }

  ~search_manager() { omp_destroy_lock(&lock); }
private:
  void get_lock() { omp_set_lock(&lock); }
  void release_lock() { omp_unset_lock(&lock); }

  tqueue_type queue;
  size_type ntask;
  answer_type ans;
  omp_lock_t lock;
  bool alloc;
};


template <typename V, typename W>
std::ostream& operator<<(std::ostream &os, search_manager<V,W> &manager) {
  typedef typename search_manager<V,W>::tqueue_type queue_type;
  typedef typename queue_type::const_iterator const_iterator;
  queue_type &q = manager.queue;
  for (const_iterator it=q.begin(); it != q.end()-1; ++it)
    os << *it << " ";
  os << *(q.end()-1);
  return os;
}

#endif
