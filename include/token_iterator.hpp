#pragma once

#include <string>
#include <cstring>
#include <functional>
#include <iostream>

#include "tstring.hpp"

template<int (*char_func)(int)>
tstring get_token(tstring& str);

template<int Char>
tstring get_token(tstring& str);

struct token_iterator {
  token_iterator(string&& s) : input(move(s)), current(input) {}
  token_iterator() : token_iterator("") {}

  std::string input;
  tstring current;
  tstring token;

  static int
  word_char(int c) { return !std::isspace(c) && c != '"' && c != '\''; }

  token_iterator&
  set_input(std::string&&, size_t position = 0);

  template<int (*F)(int)> bool
  have_token_base();

  bool
  have_token() { return have_token_base<word_char>(); }

  template<int (*F)(int)> bool
  next_token_base() { token = get_token<F>(current); return !token.untouched(); }

  bool
  next_token() { return !(token = get_token<word_char>(current)).untouched(); }
};

inline tstring get_word(tstring& str)
{ return get_token<token_iterator::word_char>(str); }

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

template<int (*char_func)(int)>
tstring get_token(tstring& str) {
  constexpr const char open_brackets[] = "([{'\"";
  constexpr const char close_brackets[] = ")]}'\"";

  auto ptr = str.begin(),
       end = str.end();
  for(; ptr < end; ptr++) {
    std::function<void(char)> match_bracket = [&](char search) {
      for(; ++ptr < end;) {
        if (*ptr == search) {
          ptr++;
          return;
        }
        if (auto type = strchr(open_brackets, *ptr); type)
          match_bracket(close_brackets[type - open_brackets]);
        if (auto type = strchr(close_brackets, *ptr); type)
          return;
      }
    };
    if (char_func(*ptr)) {
      auto start = ptr - str.begin();
      for(; ptr < end; ptr++) {
        if (!char_func(*ptr))
          break;
        if (auto type = strchr(open_brackets, *ptr); type) {
          match_bracket(close_brackets[type - open_brackets]);
          ptr--;
        }
      }
      auto end = ptr - str.begin();
      auto result = str.interval(start, end);
      str.erase_front(end);
      return result;
    } else if (auto type = strchr(open_brackets, *ptr); type) {
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

template<int (*F)(int)>
bool token_iterator::have_token_base() {
  for(; !current.empty(); current.erase_front())
    if (F(current.front()))
      return true;
  return false;
}

template<size_t count>
int fill_tokens(tstring& str, std::array<tstring, count>& output) {
  for (int i = 0; i < count; i++) {
    if ((output[i] = get_word(str)).untouched())
      return i;
  }
  return count;
}
