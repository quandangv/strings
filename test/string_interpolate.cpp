#include "string_interpolate.hpp"
#include "test.h"

TEST(Stringinter, basic) {
  auto base = "Hello  world, !";
  vector<size_t> positions = {6, 14};
  vector<string> rep{"awful", "human"};
  EXPECT_EQ("Hello awful world, human!", interpolate(base, positions, rep));
}
