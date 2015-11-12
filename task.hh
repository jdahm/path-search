#ifndef TASK_HH
#define TASK_HH

#include <cstddef>

template <typename T>
class task_queue {
public:
  typedef T task_type;
  typedef std::size_t size_type;
  virtual void add(const task_type &t) = 0;
  virtual task_type get() = 0;
  virtual size_type size() = 0;
  virtual ~task_queue() { }
private:
  virtual void get_lock() = 0;
  virtual void release_lock() = 0;
// private:
//   virtual void allocate() = 0;
};

template <typename T, typename A>
class task_manager {
public:
  typedef T task_type;
  typedef A answer_type;
  typedef task_queue<T> tqueue_type;
  typedef typename tqueue_type::size_type size_type;

  virtual task_type get() = 0;
  virtual void give(const task_type &task) = 0;
  virtual void finish(const task_type &task) = 0;
  virtual answer_type& conclude(const answer_type &ans) = 0;
  virtual bool done() const = 0;
  virtual bool has_work() = 0;
  virtual const answer_type& answer() const = 0;

  virtual ~task_manager() { }
};


#endif
