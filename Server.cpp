#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <assert.h>

#include <iostream>

#include "Strings.h"
#include "BufferedDataReader.h"
#include "Server.h"

#define BUFSIZE 256

SimpleServer::SimpleServer(int port, int event_manger_pool_size) :
    event_manger_(event_manger_pool_size) {
  listen_socket_.reset(Network::Socket::CreateServerSocket(port, true));
}

void SimpleServer::Start() {
  if (listen_socket_->closed()) {
    fprintf(stderr, "FAIL: Server not successfully initialized yet.\n");
    return;
  }
  std::cout << "Start serving" << std::endl;
  // TODO: Submit it to event manager.
  HttpConnectionListenerHandler();
}

void SimpleServer::HttpConnectionListenerHandler() {
  struct sockaddr_in cli_addr;
  unsigned int clilen = sizeof(cli_addr);
  /* Accept actual connection from the client */
  Executors::FixedThreadPool pool(5);
  while (1) {
    int newsockfd = accept(listen_socket_->getFd(),
                           (struct sockaddr*)&cli_addr,
                           &clilen);
    int x = fcntl(newsockfd, F_GETFL, 0);
    fcntl(newsockfd, F_SETFL, x | O_NONBLOCK);
    std::cout << "*************** Accept *******************" << std::endl;
    if (newsockfd < 0) {
      std::cerr << "ERORR: Accept connection failed" << std::endl;
      continue;
    }
    // TOOD: Submit it to event manager.
    pool.AddTask(
        Base::NewCallBack(&SimpleServer::ReadRequestHandler, this, newsockfd));
  }
}

void SimpleServer::ReadRequestHandler(int fd) {
  if (messages_map_.find(fd) == messages_map_.end()) {
    messages_map_[fd] = new TestMessage();
  }
  TestMessage* message = messages_map_[fd];

  // Create a buffered data reader on the socket.
  std::unique_ptr<IO::FileDescriptorInterface> client_socket(new Network::Socket(fd));
  Utility::BufferedDataReader br(std::move(client_socket));

  // Try to parse header line "size = value\n" to get data length;
  int nread = 0;
  if (message->state() == TestMessage::INIT) {
    char c;
    while ((nread = br.Read(&c)) > 0) {
      message->WriteToBuffer(&c, 1);
      if (c == '\n') {
        std::string line(message->CharBuffer(), message->received_size());
        std::vector<std::string> result = StringUtils::SplitGreedy(line, '=');
        if (result.size() != 2) {
          message->SetState(TestMessage::ERROR);
          return;
        }
        try {
          int data_length = std::stoi(StringUtils::Strip(result[1]));
          message->ResetBuffer(data_length);
        }
        catch (std::exception& err) {
          fprintf(stderr, "Can't parse \"%s\" as int32 value", result[1].c_str());
          message->SetState(TestMessage::ERROR);
          return;
        }
        message->SetState(TestMessage::GETTING);
        break;
      }
    }
  }

  // Begin reading message data.
  if (message->state() == TestMessage::GETTING) {
    char buf[BUFSIZE];
    while ((nread = br.Read(buf, 0, BUFSIZE)) > 0) {
      message->WriteToBuffer(buf, nread);
      if (message->IsFull()) {
        message->SetState(TestMessage::FINISHREADING);
        break;
      }
    }
  }

  // TODO: Check message status and submit new handlers to event manager.
  if (message->state() == TestMessage::INIT ||
      message->state() == TestMessage::GETTING) {
    event_manger_.AddTaskWaitingReadable(fd,
        Base::NewCallBack(&SimpleServer::ReadRequestHandler, this, fd));
  }
  else if (message->state() == TestMessage::FINISHREADING) {
    event_manger_.AddTaskWaitingWritable(fd,
        Base::NewCallBack(&SimpleServer::WriteRequestHandler, this, fd));
  }
}

void SimpleServer::WriteRequestHandler(int fd) {
}


// ---------------------------- TestMessage --------------------------------- //
TestMessage::TestMessage(int size) : bufsize_(size) {
  buf = new char[BUFSIZE];
}

TestMessage::~TestMessage() {
  if (buf) {
    delete[] buf;
  }
}

void TestMessage::SetState(enum TestMessage::State state) {
  state_ = state;
}

enum TestMessage::State TestMessage::state() const {
  return state_;
}

char* TestMessage::CharBuffer() const {
  return buf;
}

void TestMessage::WriteToBuffer(const char* data, int size) {
  if (size < 0) {
    return;
  }
  int write_size =
      size < bufsize_ - received_size_? size : bufsize_ - received_size_;
  memcpy(buf + received_size_, data, write_size);
}

void TestMessage::ResetBuffer(int size) {
  if (buf) {
    delete[] buf;
  }
  buf = new char[size];
  bufsize_ = size;
  received_size_ = 0;
}
