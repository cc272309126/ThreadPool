#ifndef CALLBACK_H_
#define CALLBACK_H_

#include <functional>

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

// ------------------------------- CallBack1 -------------------------------- //
template<class A1>
class CallBack1 {
 public:
  template<class F, class... Args>
  CallBack1(F&& f, Args&&... args);

  void Run(A1 a);

 private:
  std::function<void(A1)> task;
};

template<class A1>
template<class F, class... Args>
CallBack1<A1>::CallBack1(F&& f, Args&&... args) {
  task = std::bind(std::forward<F>(f), std::forward<Args>(args)..., std::placeholders::_1);
}

template<class A1>
void CallBack1<A1>::Run(A1 a1) {
  task(a1);
}

// ----------------------------- ResultCallBack1 ---------------------------- //
template<class R, class A1>
class ResultCallBack1 {
 public:
  template<class F, class... Args>
  ResultCallBack1(F&& f, Args&&... args);

  R Run(A1 a);

 private:
  std::function<void(A1)> task;
};

template<class R, class A1>
template<class F, class... Args>
ResultCallBack1<R, A1>::ResultCallBack1(F&& f, Args&&... args) {
  task = std::bind(std::forward<F>(f), std::forward<Args>(args)..., std::placeholders::_1);
}

template<class R, class A1>
R ResultCallBack1<R, A1>::Run(A1 a1) {
  task(a1);
}

template<class F, class... Args, class A1>
auto CreateCallback(F&& f, Args&&... args)
    -> decltype(std::result_of<F(Args..., A1)>::type){
  using return_type = typename std::result_of<F(Args..., A1)>::type;
  return new ResultCallBack1<return_type, A1>();
}

}  // namespace ThreadPool

#endif /* CALLBACK_H_ */
