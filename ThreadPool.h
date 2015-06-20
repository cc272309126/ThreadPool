#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace ThreadPool {

class FixedThreadPool {
 public:
  FixedThreadPool(size_t);
  
  template<class F, class... Args>
  auto Submit(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>;

  template<class F, class... Args>
  void Execute(F&& f, Args&&... args);

  ~FixedThreadPool();
  
  void AwaitTermination();

  void Stop();

 private:
  void ThreadWorker();
  
  // need to keep track of threads so we can join them
  std::vector<std::thread> workers;
  
  // the task queue
  std::queue< std::function<void()> > tasks;
  
  // synchronization
  std::mutex queue_mutex;
  std::condition_variable condition;
  
  // stop flag
  bool stop_;

  // thread size
  int thread_size_;
};

// the constructor just launches some amount of workers
FixedThreadPool::FixedThreadPool(size_t num_threads): 
    stop_(false),
    thread_size_(num_threads) {}

// the destructor joins all threads
FixedThreadPool::~FixedThreadPool() {
  for(std::thread &worker: workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

void FixedThreadPool::ThreadWorker() {
  std::function<void()> task;
  while (1) {
    {
      std::unique_lock<std::mutex> lock(this->queue_mutex);
      this->condition.wait(lock,
                     [this] { return this->stop_ || !this->tasks.empty(); });
      if (this->stop_ && this->tasks.empty()) {
        return;
      }
      task = std::move(this->tasks.front());
      this->tasks.pop();
    }
    task();
  }
}

// add new work item to the pool
template<class F, class... Args>
auto FixedThreadPool::Submit(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type >
{  
  if (workers.size() < thread_size_) {
    workers.emplace_back(std::thread(&FixedThreadPool::ThreadWorker, this));
  }

  using return_type = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared< std::packaged_task<return_type()> >(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...)
  );

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    if(stop_) {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }
    tasks.emplace([task](){ (*task)(); });
  }
  condition.notify_one();
  return res;
}

// execute new task without returning std::future object.
template<class F, class... Args>
void FixedThreadPool::Execute(F&& f, Args&&... args) {
  Submit(std::forward<F>(f), std::forward<Args>(args)...);
}

// blocks and wait for all previously submitted tasks to be completed.
void FixedThreadPool::AwaitTermination() {
  for(std::thread &worker: workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

// Shut down the threadpool. This method does not wait for previously submitted
// tasks to be completed.
void FixedThreadPool::Stop() {
  std::unique_lock<std::mutex> lock(queue_mutex);
  stop_ = true;
}


} // namespace ThreadPool

#endif /* __THREAD_POOL_H__ */
