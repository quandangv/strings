#include "tstring.hpp"
#include "test.h"

#include <vector>
#include <tuple>

using comp_test = pair<string, string>;

TEST(TString, other) {
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

vector<comp_test> comp_tests = {
  {"", ""},
  {"123456789", "123456789"},
  {"123456789", "1234567"},
  {"123456789", "123456789abcdef"},
  {"123456789", "213456789"},
  {"423456789", "123456789"},
};
class CompTest : public Test, public WithParamInterface<comp_test> {};
INSTANTIATE_TEST_SUITE_P(TString, CompTest, ValuesIn(comp_tests));
TEST_P(CompTest, comp_equality) {
  auto a = tstring(GetParam().second);
  auto b = tstring(GetParam().first);
  EXPECT_EQ(GetParam().first < GetParam().second, a < b)
      << "First: " << GetParam().first << endl
      << "Second: " << GetParam().second;
  EXPECT_EQ(GetParam().first == GetParam().second, a == b)
      << "First: " << GetParam().first << endl
      << "Second: " << GetParam().second;
  EXPECT_EQ(GetParam().first > GetParam().second, a > b)
      << "First: " << GetParam().first << endl
      << "Second: " << GetParam().second;
}

using trim_test = pair<string, string>;
class TrimTest : public Test, public WithParamInterface<trim_test> {};
vector<trim_test> trim_tests = {
  { "   abcdef ", "abcdef" },
  { "   abc def ", "abc def" },
  { "abc def ", "abc def" },
  { "abc   def", "abc   def" },
  { "abc def\t   ", "abc def" },
  { "     \t   \t   ", "" },
  { "abc def_   ", "abc def_" },
};
INSTANTIATE_TEST_SUITE_P(TString, TrimTest, ValuesIn(trim_tests));
TEST_P(TrimTest, trim_equality) {
  auto expect = tstring(GetParam().second);
  auto reality = tstring(GetParam().first);
  EXPECT_EQ(expect, trim(reality));
  EXPECT_EQ(expect, reality);
}

struct cut_test {
  const char *src, *front, *back, *result, *front_result, *back_result;
  bool fail, front_fail, back_fail;
};
class CutTest : public Test, public WithParamInterface<cut_test> {};

vector<cut_test> cut_tests = {
  {"${abcdef}",    "${", "}",      "abcdef", "abcdef}", "${abcdef"},
  {"${abcdef]",    "${", "}",      "${abcdef]", "abcdef]", "${abcdef]", true, false, true},
  {"**abcdef**",   "**", "**",     "abcdef", "abcdef**", "**abcdef"},
  {"/*abcdef",     "/*", "*/",     "/*abcdef", "abcdef", "/*abcdef", true, false, true},
  {"rgb:FF0000",   "rgb:", "",     "FF0000", "FF0000", "rgb:FF0000"},
  {"hsl:",         "hsl:", "",     "", "", "hsl:"},
  {".cpp",         "", ".cpp",     "", ".cpp", ""},
  {"hsl:",         "hsl!", "",     "hsl:", "hsl:", "hsl:", true, true, false},
  {".cpp",         "", ".cpP",     ".cpp", ".cpp", ".cpp", true, false, true},
  {"rgb::hsl",     "rgb:", ":hsl", "", ":hsl", "rgb:"},
  {"",             "", "",         "", "", ""},
  {"abcdef",       "", "",         "abcdef", "abcdef", "abcdef"},
  {"' f a i l '",  "'", "'",       " f a i l ", " f a i l '", "' f a i l "},
  {"\" f ail '", "'", "'",        "\" f ail '", "\" f ail '", "\" f ail ", true, true, false},
};
INSTANTIATE_TEST_SUITE_P(TString, CutTest, ValuesIn(cut_tests));
TEST_P(CutTest, cut_front_back) {
  auto& testset = GetParam();
  tstring src(testset.src);
  EXPECT_EQ(cut_front_back(src, testset.front, testset.back), !testset.fail)
      << "Source: " << testset.src;
  EXPECT_EQ(src, string(testset.result))
      << "Source: " << testset.src;
}

TEST_P(CutTest, cut_front) {
  auto& testset = GetParam();
  tstring src(testset.src);
  EXPECT_EQ(cut_front(src, testset.front), !testset.front_fail)
      << "Source: " << testset.src;
  EXPECT_EQ(src, string(testset.front_result))
      << "Source: " << testset.src;
}

TEST_P(CutTest, cut_back) {
  auto& testset = GetParam();
  tstring src(testset.src);
  EXPECT_EQ(cut_back(src, testset.back), !testset.back_fail)
      << "Source: " << testset.src;
  EXPECT_EQ(src, string(testset.back_result))
      << "Source: " << testset.src;
}

struct trim_quotes_test {
  const char *src, *result;
};
class TrimQuotesTest : public Test, public WithParamInterface<trim_quotes_test> {};

vector<trim_quotes_test> trim_quotes_tests = {
  {"123456", "123456"},
  {"", ""},
  {"  123456  ", "123456"},
  {"  ' 1 2 3' ", " 1 2 3"},
  {" ' 12345 6 \"", "' 12345 6 \""},
};
INSTANTIATE_TEST_SUITE_P(TString, TrimQuotesTest, ValuesIn(trim_quotes_tests));
TEST_P(TrimQuotesTest, trim) {
  auto& testset = GetParam();
  tstring src(testset.src);
  trim_quotes(src);
  EXPECT_EQ(src, testset.result)
      << "Source: " << testset.src;
}

constexpr char test_source[] = "123456";

struct substr_test { size_t pos, end_pos, index, length; string result; };
class SubstrTest : public Test, public WithParamInterface<substr_test> {};

vector<substr_test> substr_tests = {
  {2, 6, 1, 2, "45"},
  {0, 6, 9, 1, ""},
  {1, 5, 1, 9, "345"},
};
INSTANTIATE_TEST_SUITE_P(TString, SubstrTest, ValuesIn(substr_tests));

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
INSTANTIATE_TEST_SUITE_P(TString, EraseTest, ValuesIn(erase_tests));
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
INSTANTIATE_TEST_SUITE_P(TString, erase_mid_test_intf, ValuesIn(erase_mid_tests));
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
INSTANTIATE_TEST_SUITE_P(TString, find_enclosed_test_intf, ValuesIn(find_enclosed_tests));
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
INSTANTIATE_TEST_SUITE_P(TString, find_test_intf, ValuesIn(find_tests));
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
