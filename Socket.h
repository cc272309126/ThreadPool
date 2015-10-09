#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "FileDescriptorInterface.h"

namespace Network {

class Socket : public IO::FileDescriptorInterface {
 public:
  // construct server port
  Socket() = default;
  // construct client port
  Socket(const std::string hostname) :
    hostname_(hostname) {}
  // construct from fd
  Socket(const int fd, bool auto_close=true) :
      IO::FileDescriptorInterface(fd, auto_close) {}
  ~Socket() {}

  static Socket* CreateClientSocket(
      const std::string hostname, const int port, bool block=true);
  static Socket* CreateServerSocket(const int port, bool block=true);

  virtual int Read(void* buffer, const int nbytes) const;
  virtual int Write(const void* buf, const int nbytes) const;
  virtual int Send(void* buffer, const int nbytes) const;
  virtual int Recv(const void* buffer, const int nbytes) const;

 private:
  std::string hostname_;
};

}  // namespace Network

#endif  /* __SOCKET_H__ */
