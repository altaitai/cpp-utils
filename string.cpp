#include "string.hpp"

namespace string {
std::vector<std::string> split(const std::string &line, const char delim) {
  std::vector<std::string> tokens;
  std::stringstream ss(line);
  std::string token;
  while (std::getline(ss, token, delim)) {
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }
  return tokens;
}

void uppercase(std::string *str) {
  std::transform(str->begin(), str->end(), str->begin(),
                 [](unsigned char c) { return std::toupper(c); });
}

std::string uppercase(const std::string &str) {
  std::string out(str);
  uppercase(&out);
  return out;
}

void trim(std::string *str) {
  str->erase(0, str->find_first_not_of(" \n\r\t"));
  str->erase(str->find_last_not_of(" \n\r\t") + 1);
}

std::string trim(const std::string &str) {
  std::string out(str);
  trim(&out);
  return out;
}

void trimAndUppercase(std::string *str) {
  trim(str);
  uppercase(str);
}

std::string trimAndUppercase(const std::string &str) {
  std::string out(str);
  trimAndUppercase(&out);
  return out;
}

void lowercase(std::string *str) {
  std::transform(str->begin(), str->end(), str->begin(),
                 [](unsigned char c) { return std::tolower(c); });
}

std::string lowercase(const std::string &str) {
  std::string out(str);
  lowercase(&out);
  return out;
}

void trimAndLowercase(std::string *str) {
  trim(str);
  lowercase(str);
}

std::string trimAndLowercase(const std::string &str) {
  std::string out(str);
  trimAndLowercase(&out);
  return out;
}

bool startsWith(const std::string &in, const std::string &str) {
  return (in.substr(0, str.size()) == str);
}

bool endsWith(const std::string &in, const std::string &str) {
  try {
    return (in.substr(in.size() - str.size()) == str);
  } catch (...) { return false; }
}

std::string formatInt(const int i, const int numDec) {
  std::ostringstream out;
  out << std::setfill('0') << std::setw(numDec) << i;
  return out.str();
}

bool contains(const std::vector<std::string> &in, const std::string &str) {
  return (std::find(in.begin(), in.end(), str) != in.end());
}

std::vector<std::string> filter(const std::vector<std::string> &in,
                                const std::regex &exp) {
  std::vector<std::string> out;
  for (auto const &entry : in) {
    if (std::regex_match(entry, exp)) {
      out.push_back(entry);
    }
  }
  return out;
}

bool isNullOrEmpty(const std::string &str) {
  std::string s = uppercase(str);
  return (s.empty() || s == "NULL");
}

bool isTrue(const std::string &str) {
  std::string s = uppercase(str);
  return (s == "1" || s == "TRUE");
}

std::string getDateTime(const std::string &format) {
  std::time_t rawtime;
  struct tm *timeinfo;
  char buffer[256];
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, sizeof(buffer), format, timeinfo);
  std::string str(buffer);
  return str;
}

}
