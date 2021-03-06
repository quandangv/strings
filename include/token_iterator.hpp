#pragma once

#include <string>
#include <cstring>
#include <functional>
#include <iostream>

#include "tstring.hpp"

template<int(*char_func)(int)>
tstring get_token(tstring& str);

template<int Char>
tstring get_token(tstring& str);

inline int
default_token_chars(int c) { return (!std::isspace(c) && c != '"' && c != '\'') ? 1 : 0; }

inline tstring get_word(tstring& str)
{ return get_token<default_token_chars>(str); }

template<int(*char_func)(int), size_t count>
int fill_tokens(tstring& str, std::array<tstring, count>& output);

template<size_t count>
int fill_tokens(tstring& str, std::array<tstring, count>& output);

//
// Implementation
//

template<char Char>
int match_char(int c) { return c != Char; }

template<int Char>
tstring get_token(tstring& str) {
  return get_token<match_char<Char>>(str);
}

template<int(*char_func)(int), size_t count>
int fill_tokens(tstring& str, std::array<tstring, count>& output) {
  for (size_t i = 0; i < count; i++) {
    if ((output[i] = get_token<char_func>(str)).untouched())
      return i;
  }
  return count;
}


template<size_t count>
int fill_tokens(tstring& str, std::array<tstring, count>& output) {
  return fill_tokens<default_token_chars, count>(str, output);
}

template<int (*char_func)(int)>
tstring get_token(tstring& str) {
  constexpr const char open_brackets[] = "([{'\"";
  constexpr const char close_brackets[] = ")]}'\"";

  auto ptr = str.begin(),
       end = str.end();
  for(; ptr < end; ptr++) {
    std::function<void(char)> match_bracket = [&](char search) {
      for(; ++ptr < end;) {
        // If the matching bracket is encountered, return normally.
        if (*ptr == search) {
          ptr++;
          return;
        }
        // If an opening bracket is encountered, start a recursive call to match it.
        if (auto type = strchr(open_brackets, *ptr)) {
          match_bracket(close_brackets[type - open_brackets]);
          ptr--;
          continue;
        }
        // If an unmatched closing bracket is encountered, return without consuming it.
        if (strchr(close_brackets, *ptr))
          return;
      }
    };
    if (int char_class = char_func(*ptr)) {
      auto start = ptr - str.begin();
      for(; ptr < end; ptr++) {
        if (char_class != char_func(*ptr))
          break;
        if (auto type = strchr(open_brackets, *ptr)) {
          match_bracket(close_brackets[type - open_brackets]);
          ptr--;
        }
      }
      auto end = ptr - str.begin();
      auto result = str.interval(start, end);
      str.erase_front(end);
      return result;
    } else if (auto type = strchr(open_brackets, *ptr)) {
      auto start = ptr - str.begin();
      match_bracket(close_brackets[type - open_brackets]);
      auto end = ptr - str.begin();
      auto result = str.interval(start + 1, end - 1);
      str.erase_front(end);
      return result;
    }
  }
  return tstring();
}
