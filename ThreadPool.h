#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include "CallBack.h"

namespace Executors {

class FixedThreadPool {
 public:
  FixedThreadPool(size_t);

  void AddTask(Base::Closure* task);

  ~FixedThreadPool();
  
  void AwaitTermination();

  void Stop();

 private:
  void ThreadWorker();

  // need to keep track of threads so we can join them
  std::vector<std::thread> workers;

  // the task queue
  std::queue<std::unique_ptr<Base::Closure>> tasks;
  
  // synchronization
  std::mutex worker_mutex;
  std::mutex queue_mutex;
  std::condition_variable condition;
  
  // stop flag
  bool stop_;

  // thread size
  unsigned int thread_size_;
};

} // namespace Executors

#endif /* THREAD_POOL_H_ */
