#pragma once

#include <sstream>
#include <string>
#include <vector>
//#include <concepts>

//template<typename T, typename U>
//concept iterable = requires (T a) {
//  { *a.begin() } -> std::convertible_to<U>;
//  { *++a.begin() } -> std::convertible_to<U>;
//  { *a.end() }   -> std::convertible_to<U>;
//};

template<typename StringIter, typename SizeIter>
std::string interpolate(
    const std::string& base,
    const SizeIter positions,
    const StringIter& replacements
)
//    requires iterable<StringIter, std::string> && iterable<SizeIter, size_t>
{
  std::stringstream ss;
  auto repit = replacements.begin();
  int lastpoint = 0;
  for (size_t point : positions) {
    // Check for invalid points 
    if (point > base.size() || point < lastpoint) {
      ++repit;
      continue;
    }
    if (repit == replacements.end())
      break;
    ss << base.substr(lastpoint, point - lastpoint) << *repit;
    lastpoint = point;
    ++repit;
  }
  ss << base.substr(lastpoint);
  return ss.str();
}
