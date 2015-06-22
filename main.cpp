#include <iostream>
#include <vector>
#include <chrono>
#include <exception>

#include "ThreadPool.h"

int func() {
  std::this_thread::sleep_for(std::chrono::seconds(1));
  printf("a = %d\n", 1000);
  throw std::exception();
  return 0;
}

int main(int argc, char** argv) {
  ThreadPool::FixedThreadPool pool(4);
  std::vector< std::future<int> > results;

  auto res = pool.Submit(func);
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

  return 0;
}
