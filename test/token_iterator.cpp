#include "token_iterator.hpp"
#include "test.h"

struct get_token_test : public TestWithParam<int> {
  template<int (*Func)(int)>
  void test(string input, size_t position, string token, bool result = true) {
    tstring real_token;
    ASSERT_EQ(result, get_token<Func>(input, position, real_token));
    if (result)
      ASSERT_EQ(token, real_token);
  }
};
INSTANTIATE_TEST_SUITE_P(token_iterator, get_token_test, ValuesIn({0}));
TEST_P(get_token_test, isntspace) {
  EXPECT_NO_FATAL_FAILURE(test<token_iterator::isntspace>("hello world", 0, "hello"));
  EXPECT_NO_FATAL_FAILURE(test<token_iterator::isntspace>("hello world", 1, "ello"));
  EXPECT_NO_FATAL_FAILURE(test<token_iterator::isntspace>("hello  world", 5, "world"));
  EXPECT_NO_FATAL_FAILURE(test<token_iterator::isntspace>("hello  world", 100, "world", false));
}

