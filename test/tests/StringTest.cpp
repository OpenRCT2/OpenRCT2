#include <string>
#include <tuple>
#include <utility>
#include <gtest/gtest.h>
#include <openrct2/core/String.hpp>
#include "AssertHelpers.hpp"

using TCase = std::tuple<std::string, std::string, std::string>;

class StringTest : public testing::TestWithParam<TCase>
{
};

INSTANTIATE_TEST_CASE_P(TrimData, StringTest, testing::Values(
    // input                      after Trim       after TrimStart
    TCase("string",               "string",        "string"),
    TCase("  string",             "string",        "string"),
    TCase("string  ",             "string",        "string  "),
    TCase("   some   string  ",   "some   string", "some   string  "),
    TCase("      ",               "",              ""),
    TCase(" ストリング",         "ストリング",   "ストリング"),
    TCase("ストリング ",         "ストリング",   "ストリング "),
    TCase("　ストリング　",      "ストリング",   "ストリング　"),
    TCase("　　　　",            "",              ""),
    TCase("",                     "",              ""),
    TCase("\n",                   "",              ""),
    TCase("\n\n\n\r\n",           "",              ""),
    TCase("\n\n\n\r\nstring\n\n", "string",        "string\n\n")
));
TEST_P(StringTest, Trim)
{
    auto testCase = GetParam();
    std::string input = std::get<0>(testCase);
    std::string expected = std::get<1>(testCase);
    std::string actual = String::Trim(input);
    ASSERT_EQ(expected, actual);
}

TEST_P(StringTest, TrimStart)
{
    auto testCase = GetParam();
    std::string input = std::get<0>(testCase);
    std::string expected = std::get<2>(testCase);
    std::string actual = String::TrimStart(input);
    ASSERT_EQ(expected, actual);
}

TEST_F(StringTest, Split_ByComma)
{
    auto actual = String::Split("a,bb,ccc,dd", ",");
    AssertVector<std::string>(actual, { "a", "bb", "ccc", "dd" });
}
TEST_F(StringTest, Split_ByColonColon)
{
    auto actual = String::Split("a::bb:ccc:::::dd", "::");
    AssertVector<std::string>(actual, { "a", "bb:ccc", "", ":dd" });
}
TEST_F(StringTest, Split_Empty)
{
    auto actual = String::Split("", ".");
    AssertVector<std::string>(actual, { });
}
TEST_F(StringTest, Split_ByEmpty)
{
    EXPECT_THROW(String::Split("string", ""), std::invalid_argument);
}
