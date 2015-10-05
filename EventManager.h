#ifndef EVENT_MANAGER_
#define EVENT_MANAGER_

#include <sys/epoll.h>
#include <map>
#include <mutex>
#include <thread>

#include "CallBack.h"
#include "ThreadPool.h"

namespace Executors {

class Epoll {
 public:
  
  class ActiveEvents {
   public:
    ActiveEvents(int num, const struct epoll_event* events) :
        num_(num),
        events_(events) {}
    int num() const { return num_; }
    const struct epoll_event* events() const { return events_; }

   private:
    int num_;
    const struct epoll_event* events_;
  };

  Epoll();
  ~Epoll();

  void AddMonitorReadableEvent(int fd);
  void AddMonitorWritableEvent(int fd);
  void DeleteMonitorReadableEvent(int fd);
  void DeleteMonitorWritableEvent(int fd);

  void StartPolling();
  void HandleEvents(int num, const struct epoll_event* events);
  void Stop();

  // Set awake callback. This callback runs everytime epoll_wait returns.
  // It should be set by the user and takes one argument of type
  using EpollAwakeCallBack = std::function<void(const ActiveEvents*)>;
  void SetAwakeCallBack(EpollAwakeCallBack* cb);

 private:
  void Add_Event(int fd, int event);
  void Delete_Event(int fd, int event);

  int epollfd_;
  std::unique_ptr<EpollAwakeCallBack> awake_cb_;
  static const int fd_size_;
  std::mutex mutex_;
};

class EventManager {
 public:
  EventManager(int thread_pool_size);
  EventManager(const EventManager&) = delete;
  EventManager& operator=(const EventManager&) = delete;

  void SetThreadPoolSize(size_t size);
  size_t Size();

  void AddTask(Base::Closure* task);
  void AddTaskWaitingReadable(int fd, Base::Closure* task);
  void AddTaskWaitingWritable(int fd, Base::Closure* task);

  void Start();
  void AwaitTermination();
  void Stop();

  void EpollAwakeHandler(const Epoll::ActiveEvents*);

 private:
  FixedThreadPool thread_pool_;
  Epoll epoll_;
  std::map<int, Base::Closure*> inactive_tasks_map_;
  std::mutex mutex_;
};

}  // namespace Executors

#endif  /* EVENT_MANAGER_ */