/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AssertHelpers.hpp"
#include "helpers/StringHelpers.hpp"

#include <gtest/gtest.h>
#include <openrct2/core/CodepointView.hpp>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/core/String.hpp>
#include <string>
#include <tuple>
#include <utility>

using namespace OpenRCT2;

using TCase = std::tuple<std::string, std::string, std::string>;

class StringTest : public testing::TestWithParam<TCase>
{
};

///////////////////////////////////////////////////////////////////////////////
// Tests for String::Trim
///////////////////////////////////////////////////////////////////////////////

INSTANTIATE_TEST_SUITE_P(
    TrimData, StringTest,
    testing::Values(
        // input                      after Trim       after TrimStart
        TCase("string", "string", "string"), TCase("  string", "string", "string"), TCase("string  ", "string", "string  "),
        TCase("   some   string  ", "some   string", "some   string  "), TCase("      ", "", ""),
        TCase(" ストリング", "ストリング", "ストリング"), TCase("ストリング ", "ストリング", "ストリング "),
        TCase("　ストリング　", "ストリング", "ストリング　"), TCase("　　　　", "", ""), TCase("", "", ""),
        TCase("\n", "", ""), TCase("\n\n\n\r\n", "", ""), TCase("\n\n\n\r\nstring\n\n", "string", "string\n\n")));
TEST_P(StringTest, Trim)
{
    auto testCase = GetParam();
    std::string input = std::get<0>(testCase);
    std::string expected = std::get<1>(testCase);
    std::string actual = String::trim(input);
    ASSERT_EQ(expected, actual);
}

TEST_P(StringTest, TrimStart)
{
    auto testCase = GetParam();
    std::string input = std::get<0>(testCase);
    std::string expected = std::get<2>(testCase);
    std::string actual = String::trimStart(input);
    ASSERT_EQ(expected, actual);
}

///////////////////////////////////////////////////////////////////////////////
// Tests for String::split
///////////////////////////////////////////////////////////////////////////////

TEST_F(StringTest, Split_ByComma)
{
    auto actual = String::split("a,bb,ccc,dd", ",");
    AssertVector<std::string>(actual, { "a", "bb", "ccc", "dd" });
}
TEST_F(StringTest, Split_ByColonColon)
{
    auto actual = String::split("a::bb:ccc:::::dd", "::");
    AssertVector<std::string>(actual, { "a", "bb:ccc", "", ":dd" });
}
TEST_F(StringTest, Split_Empty)
{
    auto actual = String::split("", ".");
    AssertVector<std::string>(actual, {});
}
TEST_F(StringTest, Split_ByEmpty)
{
    EXPECT_THROW(String::split("string", ""), std::invalid_argument);
}

///////////////////////////////////////////////////////////////////////////////
// Tests for String::Convert
///////////////////////////////////////////////////////////////////////////////

TEST_F(StringTest, Convert_950_to_UTF8)
{
    auto input = StringFromHex("a7d6b374aabab4c4a6e2aab0af57");
    auto expected = u8"快速的棕色狐狸";
    auto actual = String::convertToUtf8(input, OpenRCT2::CodePage::CP_950);
    ASSERT_EQ(expected, actual);
}

TEST_F(StringTest, Convert_UTF8_to_UTF8)
{
    auto input = u8"سريع|brown|ثعلب";
    auto expected = input;
    auto actual = String::convertToUtf8(input, OpenRCT2::CodePage::UTF8);
    ASSERT_EQ(expected, actual);
}

TEST_F(StringTest, Convert_Empty)
{
    auto input = "";
    auto expected = input;
    auto actual = String::convertToUtf8(input, OpenRCT2::CodePage::CP_1252);
    ASSERT_EQ(expected, actual);
}

///////////////////////////////////////////////////////////////////////////////
// Tests for String::toUpper
///////////////////////////////////////////////////////////////////////////////

TEST_F(StringTest, ToUpper_Basic)
{
    auto actual = String::toUpper("test TEST tEsT 1234");
    ASSERT_STREQ(actual.c_str(), "TEST TEST TEST 1234");
}
TEST_F(StringTest, ToUpper_Dutch)
{
    auto actual = String::toUpper(u8"fĳntjes puffend fietsen");
    ASSERT_STREQ(actual.c_str(), u8"FĲNTJES PUFFEND FIETSEN");
}
TEST_F(StringTest, ToUpper_French)
{
    auto actual = String::toUpper(u8"jusqu'à 2500 carrés de côté");
    ASSERT_STREQ(actual.c_str(), u8"JUSQU'À 2500 CARRÉS DE CÔTÉ");
}
TEST_F(StringTest, ToUpper_Greek)
{
    auto actual = String::toUpper(u8"μέχρι 2500 τετράγωνα στην άκρη");
    ASSERT_STREQ(actual.c_str(), u8"ΜΈΧΡΙ 2500 ΤΕΤΡΆΓΩΝΑ ΣΤΗΝ ΆΚΡΗ");
}
TEST_F(StringTest, ToUpper_Russian)
{
    auto actual = String::toUpper(u8"до 2500 квадратов в сторону");
    ASSERT_STREQ(actual.c_str(), u8"ДО 2500 КВАДРАТОВ В СТОРОНУ");
}
TEST_F(StringTest, ToUpper_Japanese)
{
    auto actual = String::toUpper(u8"日本語で大文字がなし");
    ASSERT_STREQ(actual.c_str(), u8"日本語で大文字がなし");
}

TEST_F(StringTest, StrLogicalCmp)
{
    auto res_logical_1 = String::logicalCmp("foo1", "foo1_2");
    auto res_logical_2 = String::logicalCmp("foo1_2", "foo1");
    auto res_1 = strcmp("foo1", "foo1_2");
    auto res_2 = strcmp("foo1_2", "foo1");
    // We only care if sign is correct, actual values might not be.
    EXPECT_GE(res_1 * res_logical_1, 1);
    EXPECT_GE(res_2 * res_logical_2, 1);
    EXPECT_NE(res_logical_1, res_logical_2);

    EXPECT_GT(String::logicalCmp("foo12", "foo1"), 0);
    EXPECT_LT(String::logicalCmp("foo12", "foo13"), 0);
    EXPECT_EQ(String::logicalCmp("foo13", "foo13"), 0);

    EXPECT_EQ(String::logicalCmp("foo13", "FOO13"), 0);

    EXPECT_LT(String::logicalCmp("A", "b"), 0);
    EXPECT_LT(String::logicalCmp("a", "B"), 0);
    EXPECT_GT(String::logicalCmp("B", "a"), 0);
    EXPECT_GT(String::logicalCmp("b", "A"), 0);

    // ^ is used at the start of a ride name to move it to the end of the list
    EXPECT_LT(String::logicalCmp("A", "^"), 0);
    EXPECT_LT(String::logicalCmp("a", "^"), 0);
    EXPECT_LT(String::logicalCmp("!", "A"), 0);
    EXPECT_LT(String::logicalCmp("!", "a"), 0);
}

TEST_F(StringTest, IEqualsU8String)
{
    EXPECT_TRUE(String::iequals(u8string{ u8"" }, u8string{ u8"" }));
    EXPECT_TRUE(String::iequals(u8string{ u8"Test" }, u8string{ u8"Test" }));
    EXPECT_TRUE(String::iequals(u8string{ u8"TesT" }, u8string{ u8"Test" }));
    EXPECT_TRUE(String::iequals(u8string{ u8"TEsT" }, u8string{ u8"Test" }));

    EXPECT_FALSE(String::iequals(u8string{ u8"Test" }, u8string{ u8"Message" }));
    EXPECT_FALSE(String::iequals(u8string{ u8"Test" }, u8string{ u8"TestMessage" }));
    EXPECT_FALSE(String::iequals(u8string{ u8"" }, u8string{ u8"Test" }));
    EXPECT_FALSE(String::iequals(u8string{ u8"Test" }, u8string{ u8"" }));
}

TEST_F(StringTest, IEqualsU8StringView)
{
    EXPECT_TRUE(String::iequals(u8string_view{ u8"" }, u8string_view{ u8"" }));
    EXPECT_TRUE(String::iequals(u8string_view{ u8"Test" }, u8string_view{ u8"Test" }));
    EXPECT_TRUE(String::iequals(u8string_view{ u8"TesT" }, u8string_view{ u8"Test" }));
    EXPECT_TRUE(String::iequals(u8string_view{ u8"TEsT" }, u8string_view{ u8"Test" }));

    EXPECT_FALSE(String::iequals(u8string_view{ u8"Test" }, u8string_view{ u8"Message" }));
    EXPECT_FALSE(String::iequals(u8string_view{ u8"Test" }, u8string_view{ u8"TestMessage" }));
    EXPECT_FALSE(String::iequals(u8string_view{ u8"" }, u8string_view{ u8"Test" }));
    EXPECT_FALSE(String::iequals(u8string_view{ u8"Test" }, u8string_view{ u8"" }));
}

TEST_F(StringTest, EqualsU8String)
{
    EXPECT_TRUE(String::equals(u8string{ u8"" }, u8string{ u8"" }));
    EXPECT_TRUE(String::equals(u8string{ u8"Test" }, u8string{ u8"Test" }));

    EXPECT_FALSE(String::equals(u8string{ u8"TesT" }, u8string{ u8"Test" }));
    EXPECT_FALSE(String::equals(u8string{ u8"TEsT" }, u8string{ u8"Test" }));
    EXPECT_FALSE(String::equals(u8string{ u8"Test" }, u8string{ u8"Message" }));
    EXPECT_FALSE(String::equals(u8string{ u8"Test" }, u8string{ u8"TestMessage" }));
    EXPECT_FALSE(String::equals(u8string{ u8"" }, u8string{ u8"Test" }));
    EXPECT_FALSE(String::equals(u8string{ u8"Test" }, u8string{ u8"" }));
}

TEST_F(StringTest, EqualsU8StringView)
{
    EXPECT_TRUE(String::equals(u8string_view{ u8"" }, u8string_view{ u8"" }));
    EXPECT_TRUE(String::equals(u8string_view{ u8"Test" }, u8string_view{ u8"Test" }));

    EXPECT_FALSE(String::equals(u8string_view{ u8"TesT" }, u8string_view{ u8"Test" }));
    EXPECT_FALSE(String::equals(u8string_view{ u8"TEsT" }, u8string_view{ u8"Test" }));
    EXPECT_FALSE(String::equals(u8string_view{ u8"Test" }, u8string_view{ u8"Message" }));
    EXPECT_FALSE(String::equals(u8string_view{ u8"Test" }, u8string_view{ u8"TestMessage" }));
    EXPECT_FALSE(String::equals(u8string_view{ u8"" }, u8string_view{ u8"Test" }));
    EXPECT_FALSE(String::equals(u8string_view{ u8"Test" }, u8string_view{ u8"" }));
}

class CodepointViewTest : public testing::Test
{
};

static std::vector<char32_t> ToVector(std::string_view s)
{
    std::vector<char32_t> codepoints;
    for (auto codepoint : CodepointView(s))
    {
        codepoints.push_back(codepoint);
    }
    return codepoints;
}

static void AssertCodepoints(std::string_view s, const std::vector<char32_t>& expected)
{
    ASSERT_EQ(ToVector(s), expected);
}

TEST_F(CodepointViewTest, CodepointView_iterate)
{
    AssertCodepoints("test", { 't', 'e', 's', 't' });
    AssertCodepoints("ゲスト", { U'ゲ', U'ス', U'ト' });
    AssertCodepoints("<🎢>", { U'<', U'🎢', U'>' });
}
