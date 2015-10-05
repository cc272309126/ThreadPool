#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"

#include "StringBuilder.h"

namespace Utility {

StringBuilder::StringBuilder() : buf_size_(DEFAULT_SIZE) {
  buf_ = new char[buf_size_];
}

StringBuilder::StringBuilder(const int size) :
    buf_size_(size),
    extend_size_(size) {
  buf_ = new char[buf_size_];
}

StringBuilder::~StringBuilder() {
  if (buf_) {
    delete[] buf_;
  }
}

const char* StringBuilder::CharArray() const {
  return buf_;
}

unsigned int StringBuilder::size() const {
  return size_;
}

std::string StringBuilder::ToString() const {
  return std::string(buf_, size_);
}

void StringBuilder::Clear() {
  size_ = 0;
  memset(buf_, 0, buf_size_);
}

void StringBuilder::Truncate(const int size) {
  if (size >= size_) {
    return;
  }
  size_ = size;
  buf_[size] = 0;
}

void StringBuilder::Append(const char c) {
  if (size_ >= buf_size_) {
    buf_size_ = buf_size_ + extend_size_;
    char* new_buf = new char[buf_size_];
    memcpy(new_buf, buf_, size_);
    memset(new_buf + size_, 0, buf_size_ - size_);
    delete buf_;
    buf_ = new_buf;
  }
  buf_[size_++] = c;
}

void StringBuilder::Append(const char* str, const int size) {
  if (str == NULL) {
    return;
  }
  if (size_ + size > buf_size_) {
    buf_size_ = size_ + size;
    char* new_buf = new char[buf_size_];
    memcpy(new_buf, buf_, size_);
    memcpy(new_buf + size_, str, size);
    size_ = buf_size_;
    delete buf_;
    buf_ = new_buf;
  }
  else {
    memcpy(buf_ + size_, str, size);
    size_ += size;    
  }
}

void StringBuilder::Append(std::string str) {
  Append(str.c_str(), str.length());
}

}
