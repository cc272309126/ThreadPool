#ifndef __FILE_DESCRIPTOR_INTERFACE_H__
#define __FILE_DESCRIPTOR_INTERFACE_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory>

namespace IO {

class FileDescriptorInterface {
 public:
  FileDescriptorInterface() = default;
  FileDescriptorInterface(const FileDescriptorInterface& fdInterface) = delete;
  void operator=(const FileDescriptorInterface& fdInterface) = delete;
  FileDescriptorInterface(const int fd, const bool auto_close);
  virtual ~FileDescriptorInterface();

  virtual void setFd(const int fd);
  virtual int getFd() const { return fd_; }

  virtual int Read(void* buffer, const int nbytes) const = 0;
  virtual int Write(const void* buf, const int nbytes) const = 0;
  virtual int Close();

  void setClosed() { closed_ = true; }
  bool closed() const { return closed_; }

 protected:
  unsigned int fd_ = -1;
  bool closed_ = true;
  bool auto_close_ = true;
};

}  // namespace IO

#endif  /* __FILE_DESCRIPTOR_INTERFACE_H__ */
