/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <string>
#include <tuple>
#include <utility>
#include <gtest/gtest.h>
#include <openrct2/core/String.hpp>
#include "AssertHelpers.hpp"
#include "helpers/StringHelpers.hpp"

using TCase = std::tuple<std::string, std::string, std::string>;

class StringTest : public testing::TestWithParam<TCase>
{
};

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

TEST_F(StringTest, Convert_950_to_UTF8)
{
    auto input = StringFromHex("a7d6b374aabab4c4a6e2aab0af57");
    auto expected = u8"快速的棕色狐狸";
    auto actual = String::Convert(input, CODE_PAGE::CP_950, CODE_PAGE::CP_UTF8);
    ASSERT_EQ(expected, actual);
}

TEST_F(StringTest, Convert_UTF8_to_932)
{
    auto input = u8"ファストブラウンフォックス";
    auto expected = StringFromHex("83748340835883678375838983458393837483488362834e8358");
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

///////////////////////////////////////////////////////////////////////////////
// Tests for String::ToUpper
///////////////////////////////////////////////////////////////////////////////

TEST_F(StringTest, ToUpper_Basic)
{
    auto actual = String::ToUpper("test TEST tEsT 1234");
    ASSERT_STREQ(actual.c_str(), "TEST TEST TEST 1234");
}
TEST_F(StringTest, ToUpper_Dutch)
{
    auto actual = String::ToUpper(u8"fĳntjes puffend fietsen");
    ASSERT_STREQ(actual.c_str(), u8"FĲNTJES PUFFEND FIETSEN");
}
TEST_F(StringTest, ToUpper_French)
{
    auto actual = String::ToUpper(u8"jusqu'à 2500 carrés de côté");
    ASSERT_STREQ(actual.c_str(), u8"JUSQU'À 2500 CARRÉS DE CÔTÉ");
}
TEST_F(StringTest, ToUpper_Greek)
{
    auto actual = String::ToUpper(u8"μέχρι 2500 τετράγωνα στην άκρη");
    ASSERT_STREQ(actual.c_str(), u8"ΜΈΧΡΙ 2500 ΤΕΤΡΆΓΩΝΑ ΣΤΗΝ ΆΚΡΗ");
}
TEST_F(StringTest, ToUpper_Russian)
{
    auto actual = String::ToUpper(u8"до 2500 квадратов в сторону");
    ASSERT_STREQ(actual.c_str(), u8"ДО 2500 КВАДРАТОВ В СТОРОНУ");
}
TEST_F(StringTest, ToUpper_Japanese)
{
    auto actual = String::ToUpper(u8"日本語で大文字がなし");
    ASSERT_STREQ(actual.c_str(), u8"日本語で大文字がなし");
}
