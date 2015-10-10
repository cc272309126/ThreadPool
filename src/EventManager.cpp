#include <stdlib.h>
#include <unistd.h>
#include <iostream>

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

EventManager::~EventManager() {
  std::cout << "deleting event manager" << std::endl;
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
  std::unique_lock<std::mutex> lock(mutex_);
  for (auto i = 0; i < active_events->num(); i++) {
    int fd = active_events->events()[i].data.fd;
    if (inactive_tasks_map_.find(fd) != inactive_tasks_map_.end()) {
      //std::cout << "find task on fd " << fd << std::endl;
      thread_pool_.AddTask(inactive_tasks_map_[fd]);
    }
  }
}

void EventManager::AddTask(Base::Closure* task) {
  thread_pool_.AddTask(task);
}

int EventManager::AddTaskWaitingReadable(int fd, Base::Closure* task) {
  std::unique_lock<std::mutex> lock(mutex_);
  int ret = epoll_.AddMonitorReadableEvent(fd);
  if (ret) {
    return ret;
  }
  if (inactive_tasks_map_.find(fd) != inactive_tasks_map_.end()) {
    delete inactive_tasks_map_[fd];
  }
  inactive_tasks_map_[fd] = task;
  return 0;
}

int EventManager::AddTaskWaitingWritable(int fd, Base::Closure* task) {
  std::unique_lock<std::mutex> lock(mutex_);
  int ret = epoll_.AddMonitorWritableEvent(fd);
  if (ret) {
    return ret;
  }
  if (inactive_tasks_map_.find(fd) != inactive_tasks_map_.end()) {
    delete inactive_tasks_map_[fd];
  }
  inactive_tasks_map_[fd] = task;
  return 0;
}

int EventManager::RemoveAwaitingTask(int fd) {
  std::unique_lock<std::mutex> lock(mutex_);
  int ret = epoll_.DeleteMonitoringEvent(fd);
  if (ret) {
    return ret;
  }
  if (inactive_tasks_map_.find(fd) != inactive_tasks_map_.end()) {
    delete inactive_tasks_map_[fd];
    inactive_tasks_map_.erase(inactive_tasks_map_.find(fd));
  }
  return 0;
}

int EventManager::ModifyTaskWaitingStatus(
    int fd, int status, Base::Closure* task) {
  std::unique_lock<std::mutex> lock(mutex_);
  int ret = epoll_.ModifyMonitorEvent(fd, status);
  if (ret) {
    return ret;
  }
  if (inactive_tasks_map_.find(fd) != inactive_tasks_map_.end()) {
    delete inactive_tasks_map_[fd];
  }
  inactive_tasks_map_[fd] = task;
  return 0;
}

void EventManager::AwaitTermination() {
  thread_pool_.AwaitTermination();
}

void EventManager::Stop() {
  thread_pool_.Stop();
}


// -------------------------------- Epoll ----------------------------------- //
const int Epoll::fd_size_ = 100;

Epoll::Epoll() {
  epollfd_ = epoll_create(fd_size_);
  std::cout << "epollfd = " << epollfd_ << std::endl;
}

Epoll::~Epoll() {
  std::cout << "deleting epoll" << std::endl;
  close(epollfd_);
}

void Epoll::StartPolling() {
  const int EPOLLEVENTS = 100;
  struct epoll_event events[EPOLLEVENTS];
  while (1) {
    auto ret = epoll_wait(epollfd_, events, EPOLLEVENTS, -1);
    std::cout << "awakening " << ret << std::endl;
    HandleEvents(ret, events);
  }
}

void Epoll::HandleEvents(int num, const struct epoll_event* events) {
  ActiveEvents active_events(num, events);
  std::unique_lock<std::mutex> lock(awake_cb_mutex_);
  if (awake_cb_) {
    (*awake_cb_)(&active_events);
  }
}

void Epoll::SetAwakeCallBack(EpollAwakeCallBack* cb) {
  std::unique_lock<std::mutex> lock(awake_cb_mutex_);
  awake_cb_.reset(cb);
}

int Epoll::AddMonitorReadableEvent(int fd) {
  // TODO: Are epoll_wait and epoll_ctl thread-safe?
  std::unique_lock<std::mutex> lock(mutex_);
  return Add_Event(fd, EPOLLIN | EPOLLONESHOT);
}

int Epoll::AddMonitorWritableEvent(int fd) {
  std::unique_lock<std::mutex> lock(mutex_);
  return Add_Event(fd, EPOLLOUT | EPOLLONESHOT);
}

int Epoll::DeleteMonitoringEvent(int fd) {
  std::unique_lock<std::mutex> lock(mutex_);
  return Delete_Event(fd, EPOLLIN | EPOLLONESHOT);
}

int Epoll::ModifyMonitorEvent(int fd, int status) {
  std::unique_lock<std::mutex> lock(mutex_);
  return Modify_Event(fd, status);
}

int Epoll::Add_Event(int fd, int event) {
  struct epoll_event ev;
  ev.events = event;
  ev.data.fd = fd;
  int ret = epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev);
  return ret;
}

int Epoll::Delete_Event(int fd, int event) {
  struct epoll_event ev;
  ev.events = event;
  ev.data.fd = fd;
  int ret = epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev);
  return ret;
}

int Epoll::Modify_Event(int fd, int event) {
  struct epoll_event ev;
  ev.events = event;
  ev.data.fd = fd;
  int ret = epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev);
  return ret;
}


}  // namespace Executors
