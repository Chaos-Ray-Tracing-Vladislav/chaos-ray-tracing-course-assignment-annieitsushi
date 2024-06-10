#ifndef UTILS_H
#define UTILS_H

#include <algorithm>

int clamp(int number, int lower, int upper) {
  return std::max(lower, std::min(number, upper));
}

#endif
