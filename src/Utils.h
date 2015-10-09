#ifndef _UTILS
#define _UTILS

class Utils {
 public:
  template<typename T>
  static T Max(T t1, T t2) { return t1 > t2? t1 : t2; }

  template<typename T>
  static T Min(T t1, T t2) { return t1 < t2? t1 : t2; }
};

#endif