#include "tstring.hpp"

tstring operator ""_ts(const char* str, size_t len) {
  return tstring(str, 0, len);
}

size_t find(const tstring& ts, char ch) {
  auto result = std::find(ts.begin(), ts.end(), ch);
  return result == ts.end() ? tstring::npos : result - ts.begin();
}

size_t find(const tstring& ts, const char* str) {
  for (auto it = ts.begin(); it != ts.end(); ++it)
    if (strchr(str, *it))
      return it - ts.begin();
  return tstring::npos;
}

size_t rfind(const tstring& ts, char ch) {
  auto result = std::find(ts.rbegin(), ts.rend(), ch);
  return result == ts.rend() ? tstring::npos : result.base() - ts.begin() - 1;
}

tstring::operator string() const {
  return string(&*begin(), size());
}

bool find_enclosed(tstring& str, string& source,
                   const string& start_group,
                   const string& end_group,
                   size_t& start, size_t& end) {
  return find_enclosed(str, source, start_group, start_group, end_group, start, end);
}

bool find_enclosed(tstring& str, string& src,
                   const string& start_group, const string& false_start_group,
                   const string& end_group,
                   size_t& start, size_t& end) {
  size_t opening_count = 0;
  start = string::npos;
  auto ptr = str.begin(),
       start_limit = str.end() + 1 - start_group.size() - end_group.size(),
       false_start_limit = str.end() + 1 - false_start_group.size() - end_group.size(),
       end_limit = str.end() + 1 - end_group.size();
  for(; ptr < end_limit; ptr++) {
    auto compare = [&](const string& str) {
      return std::equal(str.data(), str.data() + str.size(), ptr);
    };

    if (ptr < start_limit && compare(start_group)) {
      // Detected start group
      if (ptr > str.begin() && ptr[-1] == '\\') {
        str.erase(src, --ptr - str.begin(), 1);
      } else if (opening_count++ == 0)
        start = ptr - str.begin();
      ptr += start_group.size() - 1;

    } else if (ptr < false_start_limit && compare(false_start_group)) {
      // Detected false start group
      if (opening_count)
        opening_count++;
      ptr += false_start_group.size() - 1;

    } else if (compare(end_group)) {
      // Detected end group
      ptr += end_group.size() - 1;
      if (opening_count > 0 && --opening_count == 0 && start != string::npos) {
        end = ptr - str.begin() + 1;
        return true;
      }
    }
  }
  return false;
}

size_t ltrim(tstring& ts, const char* trim_char) {
  auto ptr = ts.begin(), end = ts.end();
  for(; ptr != end && strchr(trim_char, *ptr) != nullptr; ptr++);
  auto count = ptr - ts.begin();
  ts.erase_front(count);
  return count;
}

size_t rtrim(tstring& ts, const char* trim_char) {
  auto ptr = ts.rbegin(), end = ts.rend();
  for(; ptr != end && strchr(trim_char, *ptr) != nullptr; ptr++);
  auto count = ptr - ts.rbegin();
  ts.erase_back(count);
  return count;
}

tstring& trim(tstring& ts, const char* trim_char) {
  ltrim(ts, trim_char);
  rtrim(ts, trim_char);
  return ts;
}

tstring& trim_quotes(tstring& ts) {
  trim(ts);
  cut_front_back(ts, "'"_ts, "'"_ts);
  cut_front_back(ts, "\""_ts, "\""_ts);
  return ts;
}

bool cut_front(tstring& ts, tstring front) {
  auto lfront = front.size();
  if (lfront > ts.length() || !std::equal(front.begin(), front.begin() + lfront, ts.begin()))
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

bool cut_back(tstring& ts, tstring back) {
  auto lback = back.size();
  if (lback > ts.length() || !std::equal(back.begin(), back.begin() + lback, ts.end() - lback))
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

bool cut_front_back(tstring& ts, tstring front, tstring back) {
  auto lfront = front.size();
  auto lback = back.size();
  if (lfront + lback > ts.length() || !std::equal(front.begin(), front.begin() + lfront, ts.begin()) || !std::equal(back.begin(), back.begin() + lback, ts.end() - lback))
    return false;
  ts.erase_front(lfront);
  ts.erase_back(lback);
  return true;
}
