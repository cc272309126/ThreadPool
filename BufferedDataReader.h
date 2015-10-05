#ifndef __BUFFERED_DATA_READER__
#define __BUFFERED_DATA_READER__

#include <iostream>
#include <string>
#include <memory>

#include "FileDescriptorInterface.h"

namespace Utility {

class BufferedDataReader {
 public:
  // Constructors
  BufferedDataReader(
      std::unique_ptr<IO::FileDescriptorInterface> fd, int bufSize);

  BufferedDataReader(std::unique_ptr<IO::FileDescriptorInterface> fd);

  virtual ~BufferedDataReader() {
    //std::cout << "deleting BufferedDataReader\n";
    Close();
  }

  // read a byte
  int Read(char* c);

  // read a chunk of data from buffer
  int Read(char* buf, int off, const int len);

  // Read one line from buffer
  std::string* ReadLine(std::string* str);

  // Close the pipe
  int Close();

  // Compare contents of two buffers.
  static bool dataCompare(const char* buf1,
                          const int off1,
                          const char* buf2,
                          const int off2,
                          const int len);

private:
  static const int BUFSIZE = 1024;
  static const int MAX_BUFSIZE = 4194302;

  int bufSize;
  char* buffer;
  std::unique_ptr<IO::FileDescriptorInterface> fdscrpt_;
  int head = 0;
  int tail = 0;
  int dataLen = 0; // effective data lengt
  // Re-fill the internal buffer
  int refill();

  // Check user arguments.
  bool checkArgs(char* buf, const int off, const int len);
};

}  // namespace Utility

#endif /* __BUFFERED_DATA_READER__ */