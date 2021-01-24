#include "token_iterator.hpp"

#include <string>

// Set the string from which to extract tokens and the position to start extracting
token_iterator& token_iterator::set_input(string&& inp, size_t pos) {
  input = move(inp);
  current = tstring(inp.data(), pos, inp.size());
  return *this;
}

