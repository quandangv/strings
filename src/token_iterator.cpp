#include "token_iterator.hpp"

#include <string>

// Set the string from which to extract tokens and the position to start extracting
token_iterator& token_iterator::set_input(string&& inp, size_t pos) {
  input = move(inp);
  current = tstring(inp.data(), pos, inp.size());
  return *this;
}

bool token_iterator::have_token() {
  for(; !current.empty(); current.erase_front())
    if (!isspace(current.front()))
      return true;
  return false;
}

// Advance to the next token made up of non-space characters
bool token_iterator::next_token() {
  token = get_token<word_char>(current);
  return !token.untouched();
}
