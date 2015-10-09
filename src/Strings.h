#ifndef __STRINGS_H_
#define __STRINGS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <memory>

class StringUtils {
 public:
  static bool IsCapitalLetter(const char c) {
    return c >= 'A' && c <= 'Z';
  }

  static bool IsLowerCaseLetter(const char c) {
    return c >= 'a' && c <= 'z';
  }

  static std::string Upper(std::string str) {
    char c[str.length()];
    for (unsigned int i = 0; i < str.length(); i++) {
      if (str[i] >= 'a' && str[i] <= 'z') {
        c[i] = str[i] - 32;
      }
      else {
        c[i] = str[i];
      }
    }
    return std::string(c, str.length());
  }

  static std::string Lower(std::string str) {
    char c[str.length()];
    for (unsigned int i = 0; i < str.length(); i++) {
      if (str[i] >= 'A' && str[i] <= 'Z') {
        c[i] = str[i] + 32;
      }
      else {
        c[i] = str[i];
      }
    }
    return std::string(c, str.length());
  }

  static bool IsLetterOrDigitOrUnderScore(const char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') ||
           (c == '_');
  }

  static std::string Strip(std::string str);
  static std::string Strip(std::string str, std::string match);

  static bool StartWith(std::string& str, std::string match);
  static bool EndWith(std::string& str, std::string match);

  static std::vector<std::string> Split(const std::string& str, const char c);
  static std::vector<std::string> Split(
      const std::string& str, const std::string& match);

  static std::vector<std::string> SplitGreedy(const std::string& str, const char c);
  static std::vector<std::string> SplitGreedy(const std::string& str,
                                              const std::string& match);

  static bool IsSingleWord(const std::string& str);

  static std::string StrCat(std::vector<std::string>, unsigned int start);

  static std::string IntToHexString(int i);

  static int findFirstMatch(std::string str, std::string match);
  static int findFirstMatch(std::string str, std::string match, int offset);
  static int findLastMatch(std::string str, std::string match);
  
  static void replaceWith(std::string& str, const char old, const char match);
  static const std::string replaceWith(std::string& str,
                                        const std::string& old,
                                        const std::string& match);

  // extract tokens from a string.
  static std::vector<std::string> ExtractTokens(
      std::string* str, char start, char end);

};

#endif /* __STRINGS_H_ */
