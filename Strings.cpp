#include <sstream>

#include "Strings.h"
#include "Utils.h"
#include "StringBuilder.h"

std::string StringUtils::Strip(std::string str) {
  if (str.length() <= 1) {
    return str;
  }
  unsigned int i, j;
  for (i = 0; i < str.length(); i++) {
    if (str[i] != ' ' && str[i] != '\t') {
      break;
    }
  }
  for (j = str.length() - 1; j >= 0; j--) {
    if (str[j] != ' ' && str[j] != '\t') {
      break;
    }
  }
  return str.substr(i, j + 1 - i);
}

std::string StringUtils::Strip(std::string str, std::string match) {
  std::string str1 = Strip(str);
  if (match.length() == 0) {
    return str1;
  }
  // strip head.
  int limit = Utils::Min(str.length(), match.length());
  int i = 0;
  for (; i < limit; i++) {
    if (str[i] != match[i]) {
      break;
    }
  }

  int j = str.length() - 1, k = match.length() - 1;
  for (; j >= 0 && k >= 0; j--, k--) {
    if (str[j] != match[k]) {
      break;
    }
  }
  // printf("i = %d, j = %d\n", i, j);
  return str.substr(i, j + 1 - i);
}

bool StringUtils::StartWith(std::string& str, std::string match) {
  if (match.length() > str.length()) {
    return false;
  }
  for (unsigned int i = 0; i < match.length(); i++) {
    if (str[i] != match[i]) {
      return false;
    }
  }
  return true;
}

bool StringUtils::EndWith(std::string& str, std::string match) {
  if (match.length() > str.length()) {
    return false;
  }
  int strlength = str.length(), matchlength = match.length();
  for (unsigned int i = 0; i < match.length(); i++) {
    if (str[strlength - 1 - i] != match[matchlength - 1 - i]) {
      return false;
    }
  }
  return true;
}

std::vector<std::string> StringUtils::Split(const std::string& str, const char c) {
  std::vector<std::string> result;
  unsigned int start = 0;
  for (unsigned int i = 0; i < str.length(); i++) {
    if (str[i] == c) {
      result.push_back(str.substr(start, i - start));
      start = i + 1;
    }
  }
  result.push_back(str.substr(start, str.length() - start));
  return result;
}

std::vector<std::string> StringUtils::Split(const std::string& str,
                                            const std::string& match) {
  std::vector<std::string> result;
  if (match.length() == 0 || str.length() <= match.length()) {
    return result;
  }

  unsigned int start = 0;
  for (unsigned int i = 0; i <= str.length() - match.length(); i++) {
    if (str.substr(i, match.length()) == match) {
      result.push_back(str.substr(start, i - start));
      start = i + match.length();
    }
  }
  result.push_back(str.substr(start, str.length() - start));
  return result;
}

std::vector<std::string> StringUtils::SplitGreedy(const std::string& str,
                                                  const char c) {
  std::vector<std::string> result;
  unsigned int start = 0;
  for (unsigned int i = 0; i < str.length(); i++) {
    if (str[i] == c) {
      std::string piece = str.substr(start, i - start);
      if (piece.length() > 0) {
        result.push_back(piece);
      }
      start = i + 1;
    }
  }
  if (start < str.length()) {
    result.push_back(str.substr(start, str.length() - start));
  }
  return result;
}

std::vector<std::string> StringUtils::SplitGreedy(const std::string& str,
                                                  const std::string& match) {
  std::vector<std::string> result;
  if (match.length() == 0 || str.length() <= match.length()) {
    return result;
  }

  unsigned int start = 0;
  for (unsigned int i = 0; i <= str.length() - match.length(); i++) {
    if (str.substr(i, match.length()) == match) {
      std::string piece = str.substr(start, i - start);
      if (piece.length() > 0) {
        result.push_back(piece);
      }
      start = i + match.length();
    }
  }
  if (start < str.length()) {
    result.push_back(str.substr(start, str.length() - start));
  }
  return result;
}

bool StringUtils::IsSingleWord(const std::string& str) {
  return SplitGreedy(str, ' ').size() == 0;
}

std::string
StringUtils::StrCat(std::vector<std::string> v, unsigned int start = 0) {
  std::string result = "";
  for (unsigned int i = start; i < v.size(); i++) {
    result += v[i];
  }
  return result;
}

std::string StringUtils::IntToHexString(int i) {
  std::stringstream stream;
  stream << "0x" 
         << std::hex << i;
  return stream.str();
}

int StringUtils::findFirstMatch(std::string str, std::string match) {
  if (match.length() > str.length()) {
    return -1;
  }
  for (unsigned int i = 0; i <= str.length() - match.length(); i++) {
    if (str.substr(i, match.length()) == match) {
      return i;
    }
  }
  return -1;
}

int StringUtils::findFirstMatch(
    std::string str, std::string match, int offset) {
  if (match.length() - offset > str.length()) {
    return -1;
  }
  for (unsigned int i = offset; i <= str.length() - match.length(); i++) {
    if (str.substr(i, match.length()) == match) {
      return i;
    }
  }
  return -1;
}

int StringUtils::findLastMatch(std::string str, std::string match) {
  if (match.length() > str.length()) {
    return -1;
  }
  for (unsigned int i = str.length() - match.length(); i >= 0; i--) {
    if (str.substr(i, match.length()) == match) {
      return i;
    }
  }
  return -1;
}

void StringUtils::replaceWith(
    std::string& str, const char old, const char rep) {
  for (unsigned int i = 0; i < str.length(); i++) {
    if (str[i] == old) {
      str[i] = rep;
    }
  }
}

const std::string StringUtils::replaceWith(
    std::string& str, const std::string& old, const std::string& rep) {
  Utility::StringBuilder str_builder;
  for (unsigned int i = 0; i <= str.length() - old.length(); i++) {
    if (str.substr(i, old.length()) == old) {
      str_builder.Append(rep);
    }
    else {
      str_builder.Append(str[i]);
    }
  }
  return str_builder.ToString();
}

std::vector<std::string> StringUtils::ExtractTokens(
    std::string* str, char start, char end) {
  Utility::StringBuilder str_builder;
  std::vector<std::string> result;
  bool matching = false;
  int matching_start = 0;
  for (unsigned int i = 0; i < str->length(); i++) {
    if (!matching) {
      if ((*str)[i] == start) {
        matching = true;
        matching_start = i;
      }
      else {
        str_builder.Append((*str)[i]);
      }
    }
    else {
      if ((*str)[i] == end) {
        result.push_back(str->substr(matching_start, i + 1 - matching_start));
        matching = false;
      }
    }
  }
  *str = str_builder.ToString();
  return result;
}