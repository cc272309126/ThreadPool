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
  enum State {
    IDLE,
    RUNNING,
    STOP,
  };

  FixedThreadPool();
  FixedThreadPool(size_t);
  FixedThreadPool(const FixedThreadPool&) = delete;
  FixedThreadPool& operator=(const FixedThreadPool&) = delete;

  void SetPoolSize(size_t);
  size_t Size() const;

  void AddTask(Base::Closure* task);

  ~FixedThreadPool();
  
  void AwaitTermination();

  void Start();
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
  State state_;

  // thread size
  unsigned int thread_size_;
};

} // namespace Executors

#endif /* THREAD_POOL_H_ */
