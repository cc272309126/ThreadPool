#include <iostream>

#include "ThreadPool.h"

namespace Executors {

FixedThreadPool::FixedThreadPool(): 
    state_(IDLE),
    thread_size_(4) {
}

// Constructor does nothing. Threads are created when new task submitted.
FixedThreadPool::FixedThreadPool(size_t num_threads):
    state_(IDLE),
    thread_size_(num_threads) {
}

void FixedThreadPool::SetPoolSize(size_t size) {
  thread_size_ = size;
}

size_t FixedThreadPool::Size() const {
  return thread_size_;
}

// Destructor joins all threads
FixedThreadPool::~FixedThreadPool() {
  for(std::thread &worker: workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

void FixedThreadPool::Start() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    state_ = RUNNING;
    unsigned int num_working_threads_ =
        thread_size_ < tasks.size()? thread_size_ : tasks.size();
    for (unsigned int i = workers.size(); i < num_working_threads_; i++) {
      workers.emplace_back(std::thread(&FixedThreadPool::ThreadWorker, this));
    }
  }
  condition.notify_all();
}

// Thread worker
void FixedThreadPool::ThreadWorker() {
  Base::Closure* task;
  while (1) {
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      condition.wait(lock,
                     [this] { return state_ == STOP || !tasks.empty(); });
      if (state_ == STOP && tasks.empty()) {
        return;
      }
      task = (tasks.front()).release();
      tasks.pop();
    }
    task->Run();
  }
}

// Add new work item to the pool
void FixedThreadPool::AddTask(Base::Closure* task) {
  {
    std::unique_lock<std::mutex> lock(worker_mutex);
    if (state_ == RUNNING && workers.size() < thread_size_) {
      workers.emplace_back(std::thread(&FixedThreadPool::ThreadWorker, this));
    }
  }

  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    if (state_ == STOP) {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }
    tasks.emplace(std::unique_ptr<Base::Closure>(task));
  }
  condition.notify_one();
}

// Blocks and wait for all previously submitted tasks to be completed.
void FixedThreadPool::AwaitTermination() {
  condition.notify_all();
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
    state_ = STOP;
  }
  condition.notify_all();
}

}  // namespace Executors
