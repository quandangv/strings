#include "tstring.hpp"

#include <iostream>
#include <cctype>
#include <algorithm>

using namespace std;

// Set the content of the tstring from a string
// The tstring is then dependent on that string
void tstring::set(const string& s) {
  this->data = s.data();
  pos = 0;
  end_pos = s.length();
}

tstring tstring::interval(size_t start, size_t end) const {
  if (start >= end_pos)
    return tstring(data, end_pos, end_pos);
  return tstring(data, pos + start, min(end_pos, pos + end));
}

void tstring::erase(string& source, size_t off, size_t length) {
  auto s = size();
  // Try to use linear-time methods first
  if (off < s) {
    if (off == 0)
      erase_front(length);
    else if (length + off > s) {
      set_length(off);
    } else {
      source.erase(pos + off, length);
      end_pos -= length;
      data = source.data();
    }
  }
}

void tstring::replace(string& source, size_t off, size_t length, const string& replacement) {
  source.replace(pos + off, length, replacement);
  end_pos += replacement.size() - length;
  data = source.data();
}

void tstring::merge(const tstring& other) {
  if (data != other.data)
    throw std::invalid_argument("tstring::merge: The other tstring must point to the same base string");
  pos = std::min(pos, other.pos);
  end_pos = std::max(end_pos, other.end_pos);
}
