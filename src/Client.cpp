#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <cmath>
#include <vector>
#include <set>
#include <thread>
#include <algorithm>
#include <string.h>

#include "Client.h"
#include "ThreadPool.h"

SimpleClient::SimpleClient(const std::string hostname, const int port) :
    hostname_(hostname),
    port_(port) {
  srand(time(NULL));
}

int SimpleClient::RunOneSession() {
  Network::Socket* socket =
      Network::Socket::CreateClientSocket(hostname_, port_);
  if (!socket) {
    return -1;
  }

  std::unique_ptr<TestMessage> message(GenerateTestMessage());
  // Fill sending buffer with random data.
  int bufsize = message->bufsize();
  for (int i = 0; i < bufsize; i++) {
    char c = (char)(rand() % 256);
    message->WriteToBuffer(&c, 1);
  }

  // Send header line.
  std::string header = "size = " + std::to_string(bufsize) + "\n";
  socket->Write(header.c_str(), header.length());

  //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // Start sending chunks.
  int last_index = 0;
  for (const int index: message->SendingVector()) {
    int size = index - last_index;
    last_index = index;
    int nwrite =
        socket->Write(message->CharBuffer() + message->written_size(), size);
    if (nwrite != size) {
      std::cerr << "Client write error, expect to write " << size
                << " bytes, actual " << nwrite << std::endl;
      delete socket;
      return -1;
    }
    message->AddWrittenSize(nwrite);
    // Pause for few milliseconds to create sending interval.
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 3));
  }

  // Start receiving chunks
  char recv_buf[bufsize];
  int nread = 0, offset = 0;
  while ((nread = socket->Read(recv_buf + offset, bufsize - offset)) > 0) {
    offset += nread;
  }
  if (offset != bufsize) {
    std::cerr << "Client receive error, expect to get " << bufsize
                << " bytes, actual " << offset << std::endl;
    delete socket;
    return -1;
  }
  if (memcmp(recv_buf, message->CharBuffer(), bufsize)) {
    std::cerr << "ERROR: Received message mismatch origin" << std::endl;
    delete socket;
    return -1;
  }
  //std::cout << "Success :)" << std::endl;
  delete socket;
  return 0;
}

void SimpleClient::RunConcurrentSessions(int num) {
  Executors::FixedThreadPool pool(30);
  pool.Start();
  for (int i = 0; i < num; i++) {
    pool.AddTask(Base::NewCallBack(&SimpleClient::RunOneSession, this));
  }
  pool.Stop();
  pool.AwaitTermination();
}

TestMessage* SimpleClient::GenerateTestMessage() {
  int size = rand() % 1000 + 1;
  TestMessage* message = new TestMessage(size);

  int num = rand() % (int)sqrt(size) + 1;
  std::vector<int> vec;
  std::set<int> all;
  int i = 0;
  while (i < num - 1) {
    int index = rand() % size;
    if (all.find(index) == all.end()) {
      all.insert(index);
      vec.push_back(index);
      i++;
    }
  }
  vec.push_back(size);
  std::sort(vec.begin(), vec.end());
  message->SetSendingVector(vec);

  // std::cout << "total = " << size << std::endl;
  // for (const int i: vec) {
  //   std::cout << i << ", ";
  // }
  // std::cout << std::endl;
  return message;
}
