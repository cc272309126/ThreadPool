#include "FileDescriptorInterface.h"

namespace IO {

FileDescriptorInterface::FileDescriptorInterface(
    const int fd, const bool auto_close) : auto_close_(auto_close){
  setFd(fd);
}

FileDescriptorInterface::~FileDescriptorInterface() {
  Close();
}

void FileDescriptorInterface::setFd(const int fd) {
  if (fd > 0) {
    fd_ = fd;
    closed_ = false;
  }
}

int FileDescriptorInterface::Close() {
  if (auto_close_ && !closed_ && fd_ > 0) {
    close(fd_);
    closed_ = true;
    return 0;
  }
  return -1;
}

}  // namespace IO
