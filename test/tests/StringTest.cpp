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

///////////////////////////////////////////////////////////////////////////////
// Test Helpers
///////////////////////////////////////////////////////////////////////////////

static std::string StringFromBytes(std::initializer_list<uint8> list)
{
    std::vector<uint8> data(list);
    return std::string(data.begin(), data.end());
}

///////////////////////////////////////////////////////////////////////////////
// Tests for String::Trim
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// Tests for String::Split
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// Tests for String::Convert
///////////////////////////////////////////////////////////////////////////////

// TODO Remove when String::Convert is implemented for non-Windows platforms
#ifdef _WIN32

TEST_F(StringTest, Convert_950_to_UTF8)
{
    auto input = StringFromBytes({ 0xA7, 0xD6, 0xB3, 0x74, 0xAA, 0xBA, 0xB4, 0xC4, 0xA6, 0xE2, 0xAA, 0xB0, 0xAF, 0x57 });
    auto expected = u8"快速的棕色狐狸";
    auto actual = String::Convert(input, CODE_PAGE::CP_950, CODE_PAGE::CP_UTF8);
    ASSERT_EQ(expected, actual);
}

TEST_F(StringTest, Convert_UTF8_to_932)
{
    auto input = u8"ファストブラウンフォックス";
    auto expected = StringFromBytes({ 0x83, 0x74, 0x83, 0x40, 0x83, 0x58, 0x83, 0x67, 0x83, 0x75, 0x83, 0x89, 0x83, 0x45, 0x83, 0x93, 0x83, 0x74, 0x83, 0x48, 0x83, 0x62, 0x83, 0x4E, 0x83, 0x58 });
    auto actual = String::Convert(input, CODE_PAGE::CP_UTF8, CODE_PAGE::CP_932);
    ASSERT_EQ(expected, actual);
}

TEST_F(StringTest, Convert_UTF8_to_UTF8)
{
    auto input = u8"سريع|brown|ثعلب";
    auto expected = input;
    auto actual = String::Convert(input, CODE_PAGE::CP_UTF8, CODE_PAGE::CP_UTF8);
    ASSERT_EQ(expected, actual);
}

TEST_F(StringTest, Convert_Empty)
{
    auto input = "";
    auto expected = input;
    auto actual = String::Convert(input, CODE_PAGE::CP_1252, CODE_PAGE::CP_UTF8);
    ASSERT_EQ(expected, actual);
}

#endif
