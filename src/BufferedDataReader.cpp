#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
#include <sstream>

#include "BufferedDataReader.h"

namespace Utility {

// Constructors
BufferedDataReader::BufferedDataReader(
    std::unique_ptr<IO::FileDescriptorInterface> fd, int bufSize) {
  fdscrpt_ = std::move(fd);

  if (bufSize < 0 || bufSize > MAX_BUFSIZE) {
    bufSize = 1024;
  }
  else {
    this->bufSize = bufSize;
  }
  buffer = new char[bufSize];
}

BufferedDataReader::BufferedDataReader(
    std::unique_ptr<IO::FileDescriptorInterface> fd) {
  fdscrpt_ = std::move(fd);
  bufSize = BUFSIZE;
  buffer = new char[bufSize];
}

// read a byte
int BufferedDataReader::Read(char* c) {
  int re;
  if (dataLen == 0 && (re = refill()) <= 0) {
     return re;
  }

  *c = buffer[tail++];
  dataLen--;
  return 1;
}

// read a chunk of data from buffer
int BufferedDataReader::Read(char* buf, int off, const int len) {    
  if (!checkArgs(buf, off, len)) {
    return -1;
  }

  if (len == 0) {
    return 0;
  }
  
  int readnLeft = len;
  while (readnLeft > 0) {
    if (dataLen <= 0) {
      // Directly copy data to user buffer.
      if (readnLeft >= bufSize) {
        int re = fdscrpt_->Read(buf + off, readnLeft);
        readnLeft -= re;
        break;
      }
      else {
        // Re-fill the internal buffer.
        int nread;
        if ((nread = refill()) <= 0) {
          if (len == readnLeft) {
            return nread;
          }
          return len - readnLeft;
        }
      }
    }
    int copyLen = dataLen < readnLeft? dataLen : readnLeft;
    memcpy(buf + off, buffer + tail, copyLen);
    off += copyLen;
    tail += copyLen;
    dataLen -= copyLen;
    readnLeft -= copyLen;
    // Read length reaches expected length
    if (readnLeft == 0) {
      break;
    }
    // Nothing is available in pipe.
    // if (fd != null && fd.available() <= 0) {
    //   break;
    // }
  }
  return len - readnLeft;
}

// Read one line from buffer
std::string* BufferedDataReader::ReadLine(std::string* str) {
  // Reach file end.
  int re = 0;

  if (dataLen == 0 && (re = refill()) <= 0) {
    // fprintf(stderr, "LOG: Refill in readline() returns %d\n", re);
    return NULL;
  }

  bool eof = false, line_empty = true;
  std::stringstream sstr;
  while (true) {
    if (dataLen == 0 && refill() <= 0) {
    	//fprintf(stderr, "LOG: Refill in readline() ended.\n");
      eof = true;
      break;
    }

    if (buffer[tail] == '\n') {
      tail++;
      dataLen -= 1;
      break;
    }
    else {
      dataLen--;
      line_empty = false;
      sstr << buffer[tail++];
    }
  }

  // If end with \n and the previous char is an '\r', discard this '\r'.
  *str = sstr.str();
  if (!line_empty) {
    if (!eof && str->at(str->length() - 1) == '\r') {
      *str = str->substr(0, str->length() - 1);
    }
  }
  return str;
}

// Re-fill the internal buffer
int BufferedDataReader::refill() {
  head = 0;
  tail = 0;
  dataLen = 0;
  int readn = fdscrpt_->Read(buffer, bufSize);
  // printf("buffer read %d bytes\n", readn);
  // for (int i = 0; i < readn; i++) {
  //   printf("%c", buffer[i]);
  // }
  // printf("\n");
  if (readn <= 0) {
    dataLen = 0;
  }
  else {
    head += readn;
    dataLen = readn;
  }
  return readn;
}

// Close the pipe
int BufferedDataReader::Close() {
  delete[] buffer;
  head = tail = 0;
  dataLen = 0;
  return 0;
}

// Check user arguments.
bool BufferedDataReader::checkArgs(char* buf, const int off, const int len) {
  if (buf == NULL) {
    return false;
  }
  
  if (len < 0 || off < 0) {
    return false;
  }
  return true;
}

// Compare contents of two buffers.
bool BufferedDataReader::dataCompare(const char* buf1,
                                     const int off1,
                                     const char* buf2,
                                     const int off2,
                                     const int length) {
  for (int i = 0; i < length; i++) {
    if (buf1[off1+i] != buf2[off2+i]) {
      return false;
    }
  }
  return true;
}

}  /// namespace Utility
