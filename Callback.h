#ifndef CALLBACK_H_
#define CALLBACK_H_

namespace ThreadPool {

class CallBack {
 public:
  template<class F, class... Args>
  CallBack(F&& f, Args&&... args);
  void Run();

 private:
  std::function<void()> task;
};

template<class F, class... Args>
CallBack::CallBack(F&& f, Args&&... args) {
  task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
}

void CallBack::Run() {
  task();
}

}  // namespace ThreadPool

#endif /* CALLBACK_H_ */