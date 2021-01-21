#pragma once

#include <string>
#include <cstring>
#include <functional>
#include <iostream>

#include "tstring.hpp"

template<int (*char_func)(int)>
tstring get_token(tstring& str);

struct token_iterator {
  token_iterator(string&& s) : input(move(s)), current(input) {}
  token_iterator() : token_iterator("") {}

  std::string input;
  tstring current;
  tstring token;

  token_iterator& set_input(std::string&&, size_t position = 0);
  bool have_token();
  bool next_token();
  template<int (*F)(int)> bool next_token_base()
  { token = get_token<F>(current); return !token.untouched(); }
  static int word_char(int c) { return !std::isspace(c) && c != '"' && c != '\''; }
};

//
// Implementation
//

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

