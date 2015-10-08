#include "FileDescriptorInterface.h"

namespace IO {

FileDescriptorInterface::FileDescriptorInterface(
    const int fd, const bool auto_close) : auto_close_(auto_close){
  setFd(fd);
}

void FileDescriptorInterface::setFd(const int fd) {
  if (fd > 0) {
    fd_ = fd;
    closed_ = false;
  }
}

}  // namespace IO
