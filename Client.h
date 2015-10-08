#ifndef SIMPLE_CLIENT_
#define SIMPLE_CLIENT_

#include <iostream>
#include <mutex>
#include <thread>
#include <map>

#include "Socket.h"
#include "Server.h"

class SimpleClient {
 public:
  SimpleClient(const std::string hostname, const int port);
  void StartSession();

 private:
  TestMessage* GenerateTestMessage();

  const std::string hostname_;
  const int port_;
};


#endif  /* SIMPLE_CLIENT_ */
