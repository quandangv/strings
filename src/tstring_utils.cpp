#include "tstring.hpp"

size_t find(const tstring& ts, char ch) {
  auto result = std::find(ts.begin(), ts.end(), ch);
  return result == ts.end() ? tstring::npos : result - ts.begin();
}

size_t rfind(const tstring& ts, char ch) {
  auto result = std::find(ts.rbegin(), ts.rend(), ch);
  return result == ts.rend() ? tstring::npos : result.base() - ts.begin() - 1;
}

tstring::operator string() const {
  return string(&*begin(), size());
}

bool find_enclosed(tstring& str, string& src,
                   const string& start_group, const string& end_group,
                   size_t& start, size_t& end) {
  size_t opening_count = 0;
  auto ptr = str.begin(),
       start_limit = str.end() + 1 - std::max(start_group.size(), end_group.size()),
       end_limit = str.end() + 1 - end_group.size();
  for(; ptr < end_limit; ptr++) {
    if (ptr < start_limit && std::equal(start_group.data(), start_group.data() + start_group.size(), ptr)) {
      if (ptr > str.begin() && ptr[-1] == '\\') {
        str.erase(src, --ptr - str.begin(), 1);
      } else if (opening_count++ == 0)
        start = ptr - str.begin();
      ptr += start_group.size() - 1;
    } else if (std::equal(end_group.data(), end_group.data() + end_group.size(), ptr)) {
      ptr += end_group.size() - 1;
      if (opening_count > 0 && --opening_count == 0) {
        end = ptr - str.begin() + 1;
        return true;
      }
    }
  }
  return false;
}

tstring& ltrim(tstring& ts, const char* trim_char) {
  auto ptr = ts.begin(), end = ts.end();
  for(; ptr != end && strchr(trim_char, *ptr) != nullptr; ptr++);
  ts.erase_front(ptr - ts.begin());
  return ts;
}

tstring& rtrim(tstring& ts, const char* trim_char) {
  auto ptr = ts.rbegin(), end = ts.rend();
  for(; ptr != end && strchr(trim_char, *ptr) != nullptr; ptr++);
  ts.erase_back(ptr - ts.rbegin());
  return ts;
}

tstring& trim(tstring& ts, const char* trim_char) {
  return ltrim(ts, trim_char), rtrim(ts, trim_char);
}

tstring& trim_quotes(tstring& ts) {
  trim(ts);
  cut_front_back(ts, "'", "'");
  cut_front_back(ts, "\"", "\"");
  return ts;
}

bool cut_front(tstring& ts, const char* front) {
  auto lfront = strlen(front);
  if (lfront > ts.length() || !std::equal(front, front + lfront, ts.begin()))
    return false;
  ts.erase_front(lfront);
  return true;
}

tstring cut_front(tstring& ts, char limit) {
  if (auto lim = find(ts, limit); lim == tstring::npos) {
    return tstring();
  } else {
    auto result = ts.interval(0, lim);
    ts.erase_front(lim + 1);
    return result;
  }
}

bool cut_back(tstring& ts, const char* back) {
  auto lback = strlen(back);
  if (lback > ts.length() || !std::equal(back, back + lback, ts.end() - lback))
    return false;
  ts.erase_back(lback);
  return true;
}

tstring cut_back(tstring& ts, char limit) {
  if (auto lim = rfind(ts, limit); lim == tstring::npos) {
    return tstring();
  } else {
    auto result = ts.interval(lim + 1);
    ts.set_length(lim);
    return result;
  }
}

bool cut_front_back(tstring& ts, const char* front, const char* back) {
  auto lfront = strlen(front);
  auto lback = strlen(back);
  if (lfront + lback > ts.length() || !std::equal(front, front + lfront, ts.begin()) || !std::equal(back, back + lback, ts.end() - lback))
    return false;
  ts.erase_front(lfront);
  ts.erase_back(lback);
  return true;
}
