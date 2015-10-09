#ifndef SIMPLE_SERVER_
#define SIMPLE_SERVER_

#include <iostream>
#include <map>
#include <vector>
#include <mutex>

#include "Socket.h"
#include "EventManager.h"

class TestMessage;

class SimpleServer {
 public:
  SimpleServer(int port, int event_manger_pool_size=5);
  void Start();

 private:
  void HttpConnectionListenerHandler();
  void ReadRequestHandler(int fd);
  void WriteRequestHandler(int fd);

  std::shared_ptr<Network::Socket> listen_socket_;
  Executors::EventManager event_manger_;

  std::map<int, TestMessage*> messages_map_;
  std::mutex mutex_;
};

class TestMessage {
 public:
  enum State {
    INIT,
    READING,
    FINISHREADING,
    WRITING,
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
  int written_size() const { return written_size_; }
  void SetWrittenSize(int size) { written_size_ = size; }

  void WriteToBuffer(const char* data, int size);

  bool IsFull() const {
    return received_size_ == bufsize_;
  }

  char* CharBuffer() const;

  void SetSendingVector(const std::vector<int>& v) { vec_ = v; }
  const std::vector<int> SendingVector() const { return vec_; };

 private:
  char* buf = nullptr;
  State state_ = INIT;
  int bufsize_ = 0;
  int received_size_ = 0;
  int written_size_ = 0;
  std::vector<int> vec_;
};

#endif  /* SimpleServer */
