#pragma once

#include <string>
#include <cstring>
#include <ostream>
#include <iterator>

using std::string;

// Wrapper for C strings that provides constant-time erase front/back and substr operations
class tstring {
  // This class doesn't copy string data on construction.
  // Thus it will be invalidated as soon as the original string go out of scope
  template<typename T> int compare(const T&) const;
protected:
  const char* data;
  size_t pos, end_pos;
public:
  static constexpr size_t npos = -1;

  // const time operations
  tstring(const char* data, size_t pos, size_t end_pos) : data(data), pos(pos), end_pos(end_pos) {}
  tstring() : tstring(nullptr, 0, 0) {}
  tstring(const tstring& s) : tstring(s.data, s.pos, s.end_pos) {}
  tstring(const string& s, size_t pos) : tstring(s.data(), pos, s.size()) {}
  tstring(const string& s) : tstring(s.data(), 0, s.size()) {}

  size_t length() const { return end_pos - pos; }
  size_t size() const { return length(); }
  bool empty() const { return length() == 0; }
  bool untouched() const { return data == nullptr; }
  char front() const { return data[pos]; }
  char back() const { return data[end_pos - 1]; }
  const char* begin() const { return data + pos; }
  const char* end() const { return data + end_pos; }
  char operator[](size_t index) const { return data[pos + index]; }
  tstring interval(size_t start, size_t end) const;
  tstring interval(size_t start) const { return interval(start, end_pos); }
  void merge(const tstring& other);

  std::reverse_iterator<const char*> rbegin() const
  { return std::reverse_iterator<const char*>(data + end_pos); }

  std::reverse_iterator<const char*> rend() const
  { return std::reverse_iterator<const char*>(data + pos); }

  void set(const string&);
  void erase_front(size_t count = 1) { pos += std::min(length(), count); }
  void erase_back(size_t count = 1) { end_pos -= std::min(length(), count); }
  void set_length(size_t length) { end_pos = pos + std::min(size(), length); }

  // linear time operations
  void erase(string& source, size_t offset, size_t length = -1);
  bool operator==(const tstring& s) const { return compare(s) == 0; }
  bool operator<(const tstring& s) const { return compare(s) < 0; }
  bool operator>(const tstring& s) const { return compare(s) > 0; }
  bool operator<=(const tstring& s) const { return compare(s) <= 0; }
  bool operator>=(const tstring& s) const { return compare(s) >= 0; }
  operator string() const;
};

template<typename T>
int tstring::compare(const T& other) const {
  auto len = length();
  if (auto diff = len - other.length())
    return diff;
  for(size_t i = 0; i < len; i++)
    if (auto diff = (*this)[i] - other[i])
      return diff;
  return 0;
}

template<typename T, T (*converter)(const char*, char**)>
T convert(const tstring& ts) {
	char* pos;
  auto result = converter(ts.begin(), &pos);
  if (pos != ts.end())
    throw std::invalid_argument("Invalid numerical value");
  return result;
}

tstring operator ""_ts(const char* str, size_t len);

size_t ltrim(tstring&, const char* trim_char = "\r\n\t\v\f ");
size_t rtrim(tstring&, const char* trim_char = "\r\n\t\v\f ");
tstring& trim(tstring& ts, const char* trim_char = "\r\n\t\v\f ");
tstring& trim_quotes(tstring&);

bool cut_front(tstring&, tstring front);
bool cut_back(tstring&, tstring back);
bool cut_front_back(tstring&, tstring front, tstring back);
tstring cut_front(tstring&, char limit);
tstring cut_back(tstring&, char limit);

bool find_enclosed(tstring&, string& source,
                   const string& start_group, const string& false_start_group,
                   const string& end_group,
                   size_t& start, size_t& end);

bool find_enclosed(tstring&, string& source,
                   const string& start_group,
                   const string& end_group,
                   size_t& start, size_t& end);

size_t find(const tstring&, char);
size_t find(const tstring&, const char*);
size_t rfind(const tstring&, char);

inline tstring substr(const tstring& ts, size_t offset, size_t length)
{ return ts.interval(offset, offset + length); }

inline tstring trim_quotes(tstring&& ts)
{ return trim_quotes(ts); }

inline tstring trim(tstring&& ts, const char* trim_char = "\r\n\t\v\f ")
{ return trim(ts, trim_char); }

inline string operator+(const tstring& a, const string& b)
{ return static_cast<string>(a) + b; }

inline string operator+(const string& a, const tstring& b)
{ return a + static_cast<string>(b); }

inline std::ostream& operator<<(std::ostream& os, const tstring& ts)
{ return os << static_cast<string>(ts); }

