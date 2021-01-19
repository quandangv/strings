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

void tstring::erase_front(size_t count) {
  pos += min(length(), count);
}

void tstring::erase_back(size_t count) {
  end_pos -= min(length(), count);
}

void tstring::set_length(size_t length) {
  end_pos = pos + min(size(), length);
}

bool tstring::empty() const {
  return length() == 0;
}

bool tstring::untouched() const {
  return data == nullptr;
}

char tstring::front() const {
  return data[pos];
}

char tstring::back() const {
  return data[end_pos - 1];
}

const char* tstring::begin() const {
  return data + pos;
}

const char* tstring::end() const {
  return data + end_pos;
}

reverse_iterator<const char*> tstring::rbegin() const {
  return reverse_iterator<const char*>(data + end_pos);
}

reverse_iterator<const char*> tstring::rend() const {
  return reverse_iterator<const char*>(data + pos);
}

size_t tstring::length() const {
  return end_pos - pos;
}

char tstring::operator[](size_t index) const {
  return data[pos + index];
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
