#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include "ThreadPool.h"

namespace Executors {

// Constructor does nothing. Threads are created when new task submitted.
FixedThreadPool::FixedThreadPool(size_t num_threads): 
    stop_(false),
    thread_size_(num_threads) {}

// Destructor joins all threads
FixedThreadPool::~FixedThreadPool() {
  for(std::thread &worker: workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

// Thread worker
void FixedThreadPool::ThreadWorker() {
  Base::Closure* task;
  while (1) {
    {
      std::unique_lock<std::mutex> lock(this->queue_mutex);
      this->condition.wait(lock,
                     [this] { return this->stop_ || !this->tasks.empty(); });
      if (this->stop_ && this->tasks.empty()) {
        return;
      }
      task = (this->tasks.front()).release();
      this->tasks.pop();
    }
    task->Run();
  }
}

// Add new work item to the pool
void FixedThreadPool::AddTask(Base::Closure* task) {
  {
    std::unique_lock<std::mutex> lock(this->worker_mutex);
    if (workers.size() < thread_size_) {
      workers.emplace_back(std::thread(&FixedThreadPool::ThreadWorker, this));
    }
  }

  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    if(stop_) {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }
    tasks.emplace(std::unique_ptr<Base::Closure>(task));
  }
  condition.notify_one();
}

// Blocks and wait for all previously submitted tasks to be completed.
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
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop_ = true;
  }
  condition.notify_all();
}

}  // namespace Executors
