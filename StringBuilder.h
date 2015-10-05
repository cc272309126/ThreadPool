#ifndef STRING_BUILDR_
#define STRING_BUILDR_

#include <string>

namespace Utility {

class StringBuilder {
 public:
  StringBuilder();
  explicit StringBuilder(int size);
  ~StringBuilder();

  const char* CharArray() const;
  unsigned int size() const;
  int IsEmpty() const { return size() == 0; }
  std::string ToString() const;
  void Clear();
  void Truncate(const int size);
  void Append(const char c);
  void Append(const char* str, const int size);
  void Append(std::string str);

 private:
  static const int DEFAULT_SIZE = 128;
  int buf_size_ = 0;
  int extend_size_ = 128;
  int size_ = 0;
  char* buf_ = nullptr;
};

}

#endif /* STRING_BUILDR_ */