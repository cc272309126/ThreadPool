#include <stdlib.h>
#include <unistd.h>

#include "EventManager.h"

namespace Executors {

// ---------------------------- EventManager -------------------------------- //
EventManager::EventManager(int thread_pool_size) {
  // Create at least 3 working threads. One of them is epolling thread.
  if (thread_pool_size < 3) {
    thread_pool_size = 3;
  }
  thread_pool_.SetPoolSize(thread_pool_size);
}

void EventManager::SetThreadPoolSize(size_t size) {
  std::unique_lock<std::mutex> lock(mutex_);
  thread_pool_.SetPoolSize(size);
}

size_t EventManager::Size() {
  std::unique_lock<std::mutex> lock(mutex_);
  return thread_pool_.Size();
}

void EventManager::Start() {
  // Set awake callback for epoll.
  epoll_.SetAwakeCallBack(new Epoll::EpollAwakeCallBack(
    std::bind(&EventManager::EpollAwakeHandler, this, std::placeholders::_1)));

  // First add epolling thread to work.
  thread_pool_.AddTask(Base::NewCallBack(&Epoll::StartPolling, &epoll_));

  // Start the internal thread poll.
  thread_pool_.Start();
}

void EventManager::EpollAwakeHandler(const Epoll::ActiveEvents* active_events) {
  for (auto i = 0; i < active_events->num(); i++) {
    int fd = active_events->events()[i].data.fd;
    if (inactive_tasks_map_.find(fd) != inactive_tasks_map_.end()) {
      thread_pool_.AddTask(inactive_tasks_map_[fd]);
    }
  }
}

void EventManager::AddTask(Base::Closure* task) {
  thread_pool_.AddTask(task);
}

void EventManager::AddTaskWaitingReadable(int fd, Base::Closure* task) {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    inactive_tasks_map_[fd] = task;
  }
  // Don't lock epoll_ with mutex_. It has internal lock mechanism.
  epoll_.AddMonitorReadableEvent(fd);
}

void EventManager::AddTaskWaitingWritable(int fd, Base::Closure* task) {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    inactive_tasks_map_[fd] = task;
  }
  epoll_.AddMonitorWritableEvent(fd);
}

void EventManager::AwaitTermination() {
  thread_pool_.AwaitTermination();
}

void EventManager::Stop() {
  thread_pool_.Stop();
}


// -------------------------------- Epoll ----------------------------------- //
const int Epoll::fd_size_ = 1024;

Epoll::Epoll() {
  epollfd_ = epoll_create(fd_size_);
}

Epoll::~Epoll() {
  close(epollfd_);
}

void Epoll::StartPolling() {
  const int EPOLLEVENTS = 100;
  struct epoll_event events[EPOLLEVENTS];
  while (1) {
    auto ret = epoll_wait(epollfd_, events, EPOLLEVENTS, -1);
    HandleEvents(ret, events);
  }
}

void Epoll::HandleEvents(int num, const struct epoll_event* events) {
  ActiveEvents active_events(num, events);
  std::unique_lock<std::mutex> lock(mutex_);
  if (awake_cb_) {
    (*awake_cb_)(&active_events);
  }
}

void Epoll::SetAwakeCallBack(EpollAwakeCallBack* cb) {
  std::unique_lock<std::mutex> lock(mutex_);
  awake_cb_.reset(cb);
}

void Epoll::AddMonitorReadableEvent(int fd) {
  std::unique_lock<std::mutex> lock(mutex_);
  Add_Event(fd, EPOLLIN | EPOLLONESHOT);
}

void Epoll::AddMonitorWritableEvent(int fd) {
  std::unique_lock<std::mutex> lock(mutex_);
  Add_Event(fd, EPOLLOUT | EPOLLONESHOT);
}

void Epoll::DeleteMonitorReadableEvent(int fd) {
  std::unique_lock<std::mutex> lock(mutex_);
  Delete_Event(fd, EPOLLIN | EPOLLONESHOT);
}

void Epoll::DeleteMonitorWritableEvent(int fd) {
  std::unique_lock<std::mutex> lock(mutex_);
  Delete_Event(fd, EPOLLOUT | EPOLLONESHOT);
}

void Epoll::Add_Event(int fd, int event) {
  struct epoll_event ev;
  ev.events = event;
  ev.data.fd = fd;
  epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev);
}

void Epoll::Delete_Event(int fd, int event) {
  struct epoll_event ev;
  ev.events = event;
  ev.data.fd = fd;
  epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev);
}


}  // namespace Executors
