#include "FileDescriptorInterface.h"

namespace IO {

FileDescriptorInterface::FileDescriptorInterface(const int fd) {
  setFd(fd);
}

void FileDescriptorInterface::setFd(const int fd) {
  if (fd > 0) {
    fd_ = fd;
    closed_ = false;
  }
}

}  // namespace IO
