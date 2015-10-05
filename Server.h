#ifndef SIMPLE_SERVER_
#define SIMPLE_SERVER_

#include <iostream>
#include <map>

#include "Socket.h"
#include "EventManager.h"

class TestMessage;

class SimpleServer {
 public:
  SimpleServer(int port, int event_manger_pool_size);
  void Start();

 private:
  void HttpConnectionListenerHandler();
  void ReadRequestHandler(int fd);
  void WriteRequestHandler(int fd);

  std::shared_ptr<Network::Socket> listen_socket_;
  Executors::EventManager event_manger_;

  std::map<int, TestMessage*> messages_map_;
};

class TestMessage {
 public:
  enum State {
    INIT,
    GETTING,
    FINISHREADING,
    ERROR,
  };
 
  TestMessage() {}
  TestMessage(int size);
  ~TestMessage();

  void SetState(enum State state);
  enum State state() const;

  void ResetBuffer(int size);
  int bufsize() const { return bufsize_; }

  int received_size() const { return received_size_; }


  void WriteToBuffer(const char* data, int size);

  bool IsFull() const { return received_size_ == bufsize_; }

  char* CharBuffer() const;

 private:
  char* buf = nullptr;
  State state_ = INIT;
  int bufsize_ = 0;
  int received_size_ = 0;
};

#endif  /* SimpleServer */
