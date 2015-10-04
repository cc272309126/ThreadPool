#ifndef CALLBACK_H_
#define CALLBACK_H_

#include <functional>

namespace Base {

class CallBack;

typedef CallBack Closure;

class CallBack {
 public:
  template<class F, class... Args>
  CallBack(F&& f, Args&&... args) {
    task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
  }

  void Run() {
    task();
  }

  virtual ~CallBack() {}

 private:
  std::function<void()> task;
};


template<class F, class... Args>
CallBack* NewCallBack(F&& f, Args&&... args) {
  return new CallBack(f, args...);
}

}  // namespace Base

#endif /* CALLBACK_H_ */
