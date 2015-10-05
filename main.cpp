#include <iostream>
#include <vector>
#include <chrono>
#include <exception>
#include <map>

#include "ThreadPool.h"
#include "CallBack.h"

int func(int a) {
  std::this_thread::sleep_for(std::chrono::seconds(1));
  printf("a = %d\n", a);
  return 0;
}

void testThreadpool() {
  Executors::FixedThreadPool pool(4);
  std::vector< std::future<int> > results;

  pool.Start();
  pool.AddTask(Base::NewCallBack(func, 3));
  for(int i = 0; i < 10000; ++i) {
    pool.AddTask(new Base::CallBack([i] {
      std::cout << "hello " << i << std::endl;
      //std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "world " << i << std::endl;
      return i*i;
    }));
  }

  pool.Stop();
  pool.AwaitTermination();
  std::cout << "All tasks complted." << std::endl;
}

int main(int argc, char** argv) {
  testThreadpool();
  return 0;
}
