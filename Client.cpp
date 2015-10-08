#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cmath>
#include <vector>
#include <set>
#include <algorithm>
#include <string.h>

#include "Client.h"

SimpleClient::SimpleClient(const std::string hostname, const int port) :
    hostname_(hostname),
    port_(port) {
  srand(time(NULL));
}

void SimpleClient::StartSession() {
  Network::Socket* socket =
      Network::Socket::CreateClientSocket(hostname_, port_);

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

  // Start sending chunks.
  int last_index = 0;
  for (const int index: message->SendingVector()) {
    int size = index - last_index;
    int nwrite =
        socket->Write(message->CharBuffer() + message->written_size(), size);
    if (nwrite != size) {
      std::cerr << "Client write error, expect to write " << nwrite
                << " bytes, actual " << nwrite << std::endl;
      return;
    }
    message->SetWrittenSize(message->written_size() + nwrite);
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
    return;
  }
  if (memcmp(recv_buf, message->CharBuffer(), bufsize)) {
    std::cerr << "ERROR: Received message mismatch origin" << std::endl;
    return;
  }
  std::cout << "Success :)" << std::endl;
}

TestMessage* SimpleClient::GenerateTestMessage() {
  int size = rand() % 20 + 1;
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
  std::sort(vec.begin(), vec.end());
  message->SetSendingVector(vec);

  std::cout << "total = " << size << std::endl;
  for (const int i: vec) {
    std::cout << i << ", ";
  }
  std::cout << size << std::endl;
  return message;
}
