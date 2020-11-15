#pragma once

// System Includes
#include <functional>
#include <iostream> // std::cout, std::ostream, std::ios
#include <string>
#include <vector>

// Project Includes

// External Includes

// formatters
std::string fmt_err(const std::string &err_msg);

std::string fmt_max_hits(int max_hits);

std::string fmt_arr(std::vector<std::string> &values);

//
struct Parameters {
  int int1;
  int int2;
  int limit;
  std::string str1;
  std::string str2;

  bool operator==(const Parameters &rhs) const;

  // Parameters friend functions
  friend std::ostream &operator<<(std::ostream &os, const Parameters &p);
};

// create an std::unordered_map with user defined key
template <class T> inline void hash_combine(std::size_t &s, const T &v) {
  std::hash<T> h;
  s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

namespace std {
template <> struct hash<Parameters> {
  std::size_t operator()(const Parameters &p) const {
    std::size_t res = 0;
    hash_combine(res, p.int1);
    hash_combine(res, p.int2);
    hash_combine(res, p.limit);
    hash_combine(res, p.str1);
    hash_combine(res, p.str2);
    return res;
  }
};
} // namespace std

// std::less
template <typename A, typename B, typename U = std::less<>>
bool f(A a, B b, U u = U()) {
  return u(a, b);
}