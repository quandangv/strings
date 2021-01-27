#include "tstring.hpp"
#include "test.h"

#include <vector>
#include <tuple>

#define NOFAIL EXPECT_NO_FATAL_FAILURE

TEST(tstring, other) {
  string str = "  hello  ";

  EXPECT_EQ("  hello  ", tstring(str));

  str = "123456789";
  EXPECT_EQ(string("123456789").length(), tstring(move(str)).length());
  EXPECT_EQ(string("123456789").length(), tstring("123456789").length());
  EXPECT_EQ(string("123456789"), tstring("123456789"));

  str = "";
  EXPECT_EQ(string("").length(), tstring(move(str)).length());
  EXPECT_EQ(string("").length(), tstring("").length());
  EXPECT_EQ(string(""), tstring(""));

  tstring ts{"hello"};
  ts.set("goodbye");
  EXPECT_EQ("goodbye", ts);
  EXPECT_EQ('g', ts.front());
  EXPECT_EQ('e', ts.back());

  EXPECT_FALSE(ts.empty());
  ts.erase_front(100);
  ASSERT_TRUE(ts.empty());

  EXPECT_FALSE(ts.untouched());
  ts = tstring();
  EXPECT_TRUE(ts.untouched());
}

void test_compare(const string& str1, const string& str2) {
  auto ts1 = tstring(str1);
  auto ts2 = tstring(str2);
  ASSERT_EQ(str1 == str2, ts1 == ts2);
  ASSERT_EQ(str1 < str2, ts1 < ts2);
  ASSERT_EQ(str1 > str2, ts1 > ts2);
}
TEST(tstring, compare) {
  NOFAIL(test_compare("", ""));
  NOFAIL(test_compare("123456789", "123456789"));
  NOFAIL(test_compare("123456789", "1234567"));
  NOFAIL(test_compare("123456789", "123456789abcdef"));
  NOFAIL(test_compare("123456789", "213456789"));
  NOFAIL(test_compare("423456789", "123456789"));
}

void test_trim(const string& before, const string& after, bool quotes = false) {
  tstring ts(before), ts2(after);
  if (quotes)
    ASSERT_EQ(after, trim_quotes(ts));
  else
    ASSERT_EQ(after, trim(ts));
  ASSERT_EQ(ts, ts2);
}
TEST(tstring, trim) {
  NOFAIL(test_trim("   abcdef ", "abcdef"));
  NOFAIL(test_trim("   abc def ", "abc def"));
  NOFAIL(test_trim("abc def ", "abc def"));
  NOFAIL(test_trim("abc   def", "abc   def"));
  NOFAIL(test_trim("abc def\t   ", "abc def"));
  NOFAIL(test_trim("     \t   \t   ", ""));
  NOFAIL(test_trim("abc def_   ", "abc def_"));

  NOFAIL(test_trim("123456", "123456", true));
  NOFAIL(test_trim("", "", true));
  NOFAIL(test_trim("  123456  ", "123456", true));
  NOFAIL(test_trim("  ' 1 2 3' ", " 1 2 3", true));
  NOFAIL(test_trim(" ' 12345 6 \"", "' 12345 6 \"", true));
}

void test_cut(const string& source, const string& front, const string& back,
              const string& result, const string& front_result, const string& back_result,
              bool fail = false, bool front_fail = false, bool back_fail = false) {
  tstring ts(source);
  ASSERT_EQ(cut_front_back(ts, front.data(), back.data()), !fail);
  ASSERT_EQ(ts, result);
  ts = tstring(source);
  ASSERT_EQ(cut_front(ts, front.data()), !front_fail);
  ASSERT_EQ(ts, front_result);
  ts = tstring(source);
  ASSERT_EQ(cut_back(ts, back.data()), !back_fail);
  ASSERT_EQ(ts, back_result);
}
TEST(tstring, cut) {
  NOFAIL(test_cut("${abcdef}",    "${", "}",      "abcdef", "abcdef}", "${abcdef"));
  NOFAIL(test_cut("${abcdef]",    "${", "}",      "${abcdef]", "abcdef]", "${abcdef]", true, false, true));
  NOFAIL(test_cut("**abcdef**",   "**", "**",     "abcdef", "abcdef**", "**abcdef"));
  NOFAIL(test_cut("/*abcdef",     "/*", "*/",     "/*abcdef", "abcdef", "/*abcdef", true, false, true));
  NOFAIL(test_cut("rgb:FF0000",   "rgb:", "",     "FF0000", "FF0000", "rgb:FF0000"));
  NOFAIL(test_cut("hsl:",         "hsl:", "",     "", "", "hsl:"));
  NOFAIL(test_cut(".cpp",         "", ".cpp",     "", ".cpp", ""));
  NOFAIL(test_cut("hsl:",         "hsl!", "",     "hsl:", "hsl:", "hsl:", true, true, false));
  NOFAIL(test_cut(".cpp",         "", ".cpP",     ".cpp", ".cpp", ".cpp", true, false, true));
  NOFAIL(test_cut("rgb::hsl",     "rgb:", ":hsl", "", ":hsl", "rgb:"));
  NOFAIL(test_cut("",             "", "",         "", "", ""));
  NOFAIL(test_cut("abcdef",       "", "",         "abcdef", "abcdef", "abcdef"));
  NOFAIL(test_cut("' f a i l '",  "'", "'",       " f a i l ", " f a i l '", "' f a i l "));
  NOFAIL(test_cut("\" f ail '",   "'", "'",       "\" f ail '", "\" f ail '", "\" f ail ", true, true, false));
};
constexpr char test_source[] = "123456";

struct substr_test { size_t pos, end_pos, index, length; string result; };
class SubstrTest : public Test, public WithParamInterface<substr_test> {};

vector<substr_test> substr_tests = {
  {2, 6, 1, 2, "45"},
  {0, 6, 9, 1, ""},
  {1, 5, 1, 9, "345"},
};
INSTANTIATE_TEST_SUITE_P(tstring, SubstrTest, ValuesIn(substr_tests));

TEST_P(SubstrTest, substr) {
  auto testset = GetParam();
  tstring str;
  {
    tstring tmp(test_source, testset.pos, testset.end_pos);
    str = substr(tmp, testset.index, testset.length);
  }
  EXPECT_EQ((string)str, testset.result);
}

struct erase_test { int num; string result; };
class EraseTest : public Test, public WithParamInterface<erase_test> {};

vector<erase_test> erase_tests = {
  {2, "3456"},
  {-3, "123"},
  {1, "23456"},
  {0, "123456"},
  {6, ""},
  {9, ""},
  {-9, ""},
};
INSTANTIATE_TEST_SUITE_P(tstring, EraseTest, ValuesIn(erase_tests));
TEST_P(EraseTest, erase_front_back) {
  auto testset = GetParam();
  auto reality = tstring(test_source);
  if (testset.num < 0)
    reality.erase_back(-testset.num);
  else
    reality.erase_front(testset.num);
  EXPECT_EQ(testset.result, reality)
      << "Num: " << testset.num;
}

struct erase_mid_test { size_t off, len; string result; };
class erase_mid_test_intf : public Test, public WithParamInterface<erase_mid_test> {};

vector<erase_mid_test> erase_mid_tests = {
  {0, 0, "123456"},
  {0, 1, "23456"},
  {4, 9, "1234"},
  {1, 2, "1456"},
};
INSTANTIATE_TEST_SUITE_P(tstring, erase_mid_test_intf, ValuesIn(erase_mid_tests));
TEST_P(erase_mid_test_intf, erase_mid) {
  auto src = string(test_source);
  auto testset = GetParam();
  tstring reality(src);
  reality.erase(src, testset.off, testset.len);
  EXPECT_EQ(testset.result, reality)
      << "Start: " << testset.off << ", Length: " << testset.len;
}

struct find_enclosed_test {
  string source, start_group, false_start, end_group;
  size_t offset, result_start, result_end;
  bool fail;
};
class find_enclosed_test_intf : public Test, public WithParamInterface<find_enclosed_test> {};

vector<find_enclosed_test> find_enclosed_tests = {
  {"${2345}${9}", "${", "${", "}", 0, 0, 7},
  {"${2%{hello}345}${9}", "${", "{", "}", 0, 0, 15},
  {"${2345}${9} ", "${", "{", "}", 7, 0, 4},
  {"${2345}${9}", "${", "${", "}", 2, 5, 9},
  {"$${3456}${}", "${", "{", "}", 0, 1, 8},
  {"\\${2345}${9}", "${", "${", "}", 0, 7, 11},
  {"$$2345;$$$A;;", "$$", "{", ";", 2, 5, 10},
  {"$$2345;$A;;", "$$", "$", ";", 2, 5, 10, true},
};
INSTANTIATE_TEST_SUITE_P(tstring, find_enclosed_test_intf, ValuesIn(find_enclosed_tests));
TEST_P(find_enclosed_test_intf, find) {
  auto testset = GetParam();
  tstring src(testset.source.data(), testset.offset, testset.source.size());
  size_t start, end;
  ASSERT_EQ(find_enclosed(src, testset.source, testset.start_group, testset.false_start, testset.end_group, start, end), !testset.fail)
      << "Source: " << src<< ", start: " << testset.offset;
  if (!testset.fail) {
    ASSERT_EQ(start, testset.result_start)
        << "Source: " << src<< ", start: " << testset.offset;
    ASSERT_EQ(end, testset.result_end)
        << "Source: " << src<< ", start: " << testset.offset;
  }
}

struct find_test { string source; char c; int result, r_result; };
class find_test_intf : public Test, public WithParamInterface<find_test> {};

vector<find_test> find_tests = {
  {"123454321", '2', 1, 7},
  {"123454301", '2', 1, 1},
  {"123404321", '5', -1, -1},
  {"123454321", 0, -1, -1},
};
INSTANTIATE_TEST_SUITE_P(tstring, find_test_intf, ValuesIn(find_tests));
TEST_P(find_test_intf, find) {
  auto testset = GetParam();
  tstring src(testset.source);
  EXPECT_EQ(find(src, testset.c), testset.result)
      << "Source: " << src<< ", character: " << testset.c;
  EXPECT_EQ(rfind(src, testset.c), testset.r_result)
      << "Source: " << src<< ", character: " << testset.c;
}

struct limit_cut_test : public TestWithParam<int> {
  template<tstring (*Func)(tstring&, char)>
  void test(const string& source, char c, const string& result, const string& remain, bool fail = false) {
    tstring src(source);
    tstring real_result(Func(src, c));
    if (fail) EXPECT_TRUE(real_result.untouched())
        << "Source: " << src << ", character: " << c;
    else EXPECT_EQ(real_result, result)
        << "Source: " << src << ", character: " << c;
    EXPECT_EQ(src, remain)
        << "Source: " << src << ", character: " << c;
  }
};
INSTANTIATE_TEST_SUITE_P(tstring, limit_cut_test, ValuesIn({0}));

TEST_P(limit_cut_test, front_limit_cut) {
  test<cut_front>("hsv:ff0000", ':', "hsv", "ff0000");
  test<cut_front>(":ff0000", ':', "", "ff0000");
  test<cut_front>("ff0000", ':', "", "ff0000", true);
  test<cut_front>("hsv::ff0000", ':', "hsv", ":ff0000");
}
TEST_P(limit_cut_test, back_limit_cut) {
  test<cut_back>("ff0000:hsv", ':', "hsv", "ff0000");
  test<cut_back>("ff0000:", ':', "", "ff0000");
  test<cut_back>("ff0000", ':', "", "ff0000", true);
  test<cut_back>("ff0000::hsv", ':', "hsv", "ff0000:");
}
