#include "token_iterator.hpp"
#include "test.h"

TEST(word_char, basic) {
  EXPECT_FALSE(default_token_chars('"'));
  EXPECT_FALSE(default_token_chars('"'));
}

struct get_token_test : public TestWithParam<int> {
  template<int (*Func)(int)>
  void test(string input, size_t position, string token) {
    tstring ts(input, position);
    ASSERT_EQ(token, get_token<Func>(ts));
  }
};
INSTANTIATE_TEST_SUITE_P(token_iterator, get_token_test, ValuesIn({0}));
TEST_P(get_token_test, general) {
  EXPECT_NO_FATAL_FAILURE(test<default_token_chars>("hello world", 0, "hello"));
  EXPECT_NO_FATAL_FAILURE(test<default_token_chars>("hello world", 1, "ello"));
  EXPECT_NO_FATAL_FAILURE(test<default_token_chars>("hello  world", 5, "world"));
  EXPECT_NO_FATAL_FAILURE(test<default_token_chars>("hello  world", 100, ""));

  EXPECT_NO_FATAL_FAILURE(test<isalnum>("foo;bar", 0, "foo"));
  EXPECT_NO_FATAL_FAILURE(test<isalnum>("foo__bar", 1, "oo"));
  EXPECT_NO_FATAL_FAILURE(test<isalnum>("foo9bar", 2, "o9bar"));
}

struct token_iterator_test : public TestWithParam<int> {
  template<int(*Func)(int)>
  void test(string input, vector<string> words) {
    tstring ts(input);
    for(auto& word : words) {
      ASSERT_EQ(get_token<Func>(ts), word);
    }
    ASSERT_TRUE(get_token<Func>(ts).untouched());
  }
  void test(string input, vector<string> words) {
    test<default_token_chars>(input, words);
  }
};

int isntsemicolon(int i) {
  return i != ';';
}

INSTANTIATE_TEST_SUITE_P(token_iterator, token_iterator_test, ValuesIn({0}));
TEST_P(token_iterator_test, general) {
  EXPECT_NO_FATAL_FAILURE(test("foo; bar;", {"foo;", "bar;"}));
  EXPECT_NO_FATAL_FAILURE(test("   ", {}));
  EXPECT_NO_FATAL_FAILURE(test(" Lorem ipsum dolor sit_amet  ", {"Lorem", "ipsum", "dolor", "sit_amet"}));
  EXPECT_NO_FATAL_FAILURE(test<isntsemicolon>("  Lorem; ipsum;;dolor;sit_amet  ", {"  Lorem", " ipsum", "dolor", "sit_amet  "}));
  EXPECT_NO_FATAL_FAILURE(test("the quick'brown'fox jumps", {"the", "quick", "brown", "fox", "jumps"}));
  EXPECT_NO_FATAL_FAILURE(test("the 'brown' fox jumps", {"the", "brown", "fox", "jumps"}));
  EXPECT_NO_FATAL_FAILURE(test("the 'brown'fox jumps", {"the", "brown", "fox", "jumps"}));
  EXPECT_NO_FATAL_FAILURE(test("the (brown) fox jumps", {"the", "(brown)", "fox", "jumps"}));
  EXPECT_NO_FATAL_FAILURE(test("the (brown fox) jumps", {"the", "(brown fox)", "jumps"}));
  EXPECT_NO_FATAL_FAILURE(test("the (brown (fox)) jumps", {"the", "(brown (fox))", "jumps"}));
  EXPECT_NO_FATAL_FAILURE(test("the (brown [fox) jumps", {"the", "(brown [fox)", "jumps"}));
  EXPECT_NO_FATAL_FAILURE(test("abc(1, 2, 123)", {"abc(1, 2, 123)"}));
  EXPECT_NO_FATAL_FAILURE(test("[abc(1, 2, 123)]", {"[abc(1, 2, 123)]"}));
  EXPECT_NO_FATAL_FAILURE(test("'abc(1, 2, 123)'", {"abc(1, 2, 123)"}));
}
