#include <iostream>
#include <vector>
#include <chrono>
#include <exception>
#include <map>

#include "ThreadPool.h"
#include "Callback.h"

class RPCBase;
class SomeServiceInterface;
class A;
class B;
class Stubby;
void testThreadpool();
void testCallback();

int func(int a) {
  std::this_thread::sleep_for(std::chrono::seconds(1));
  printf("a = %d\n", a);
  //throw std::exception();
  return 0;
}

void testThreadpool() {
  ThreadPool::FixedThreadPool pool(4);
  std::vector< std::future<int> > results;

  auto res = pool.Submit(func, 3);
  //res.get(); // will get the exception
  for(int i = 0; i < 8; ++i) {
    results.emplace_back(
      pool.Submit([i] {
        std::cout << "hello " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "world " << i << std::endl;
        return i*i;
      })
    );
  }

  for(auto && result: results) {
    std::cout << result.get() << " \n";
  }
  pool.Stop();
  pool.AwaitTermination();
  std::cout << "All tasks complted." << std::endl;
}

// ===========================================================================//
class A {
 public:
  A(int value = 5): value_(value) {}
  void printValues(int another) {
    printf("value = %d, another = %d\n", value_, another);
  }
 private:
  int value_ = 5;
};

class RPCBase {
 public:
  RPCBase() = default;
};

class SomeServiceInterface : public RPCBase {
 public:
  void virtual SomeService(double para1, ThreadPool::CallBack* callback) = 0;
};

// B is a fake server
class B : public SomeServiceInterface {
 public:
  B() = default;
  void SomeService(double para1, ThreadPool::CallBack* callback) override {
    printf("Processing para1 = %lf\n", para1);
    // wait for some time and execute the call back
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (callback) {
      callback->Run();
      delete callback;
    }
  }
};

class Stubby {
 public:
  Stubby() = default;
  void rebind(std::string name, RPCBase* rpcserver) {
    map_[name] = rpcserver;
  }
  RPCBase* lookup(std::string name) {
    return map_.find(name)->second;
  }
 private:
  std::map<std::string, RPCBase*> map_;
};

int AddIntegers(int a, int b) {
  return a + b;
}

void PrintIntegers(int a, int b) {
  std::cout << "arg1 = " << a << ", arg2 = " << b << std::endl;
}

void testCallback() {
  // Local test.
  ThreadPool::CallBack1<int>* callback1 = new ThreadPool::CallBack1<int>(&func);
  callback1->Run(5);
  delete callback1;

  ThreadPool::ResultCallBack1<int, int>* callback2 =
      new ThreadPool::CreateCallback(AddIntegers, 1);
  int c = callback2->Run(2);
  std::cout << c << std::endl;

  ThreadPool::ResultCallBack1<void, int>* callback3 =
      new ThreadPool::CreateCallback(PrintIntegers, 1);
  callback3->Run(2);

  // Mock RPC Service.
  B server;
  Stubby stub;
  stub.rebind("Bserver", &server);
  // client get RPC service from stub
  A a;
  SomeServiceInterface* service = (SomeServiceInterface*)stub.lookup("Bserver");
  service->SomeService(1.5, new ThreadPool::CallBack(&A::printValues, &a, 7));
}

int main(int argc, char** argv) {
  // testThreadpool();
  testCallback();
  return 0;
}
