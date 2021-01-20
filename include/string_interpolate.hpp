#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <concepts>

template<typename T, typename U>
concept iterable = requires (T a) {
  { *a.begin() } -> std::convertible_to<U>;
  { *a.begin()++ } -> std::convertible_to<U>;
  { *a.end() }   -> std::convertible_to<U>;
};

template<typename T>
std::string interpolate(
    const std::string& base,
    const std::vector<size_t> positions,
    const T& replacements
) requires iterable<T, std::string> {
  std::stringstream ss;
  auto repit = replacements.begin();
  int lastpoint = 0;
  for (int point : positions) {
    // Check for invalid points 
    if (point > base.size() || point < lastpoint)
      continue;
    if (repit == replacements.end())
      break;
    ss << base.substr(lastpoint, point - lastpoint) << *repit++;
    lastpoint = point;
  }
  ss << base.substr(lastpoint);
  return ss.str();
}
