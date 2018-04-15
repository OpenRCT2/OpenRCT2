#include "helpers/StringHelpers.hpp"
#include "openrct2/localisation/Localisation.h"
#include <gtest/gtest.h>

class Localisation : public testing::Test
{
};

///////////////////////////////////////////////////////////////////////////////
// Tests for rct2_to_utf8
///////////////////////////////////////////////////////////////////////////////

TEST_F(Localisation, RCT2_to_UTF8_UK)
{
    auto input = "The quick brown fox";
    auto expected = u8"The quick brown fox";
    auto actual = rct2_to_utf8(input, RCT2_LANGUAGE_ID_ENGLISH_UK);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, RCT2_to_UTF8_JP)
{
    auto input = StringFromHex("83748340835883678375838983458393837483488362834e8358");
    auto expected = u8"ファストブラウンフォックス";
    auto actual = rct2_to_utf8(input, RCT2_LANGUAGE_ID_JAPANESE);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, RCT2_to_UTF8_ZH_TW)
{
    auto input = StringFromHex("a7d6b374aabab4c4a6e2aab0af57");
    auto expected = u8"快速的棕色狐狸";
    auto actual = rct2_to_utf8(input, RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, RCT2_to_UTF8_PL)
{
    auto input = StringFromHex("47F372736b6120446ff76b692054e6637a6f7779");
    auto expected = u8"Górska Dołki Tęczowy";
    auto actual = rct2_to_utf8(input, RCT2_LANGUAGE_ID_ENGLISH_UK);
    ASSERT_EQ(expected, actual);
}

///////////////////////////////////////////////////////////////////////////////
// Tests for utf8_to_rct2
///////////////////////////////////////////////////////////////////////////////

TEST_F(Localisation, UTF8_to_RCT2_Basic)
{
    auto input = u8"à l'époque était";
    auto expected = StringFromHex("e0206c27e9706f71756520e974616974");
    auto actual = utf8_to_rct2(input);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, UTF8_to_RCT2_ChineseTraditional)
{
    auto input = u8"$: 快速的棕色狐狸";
    auto expected = StringFromHex("243a20ff5febff901fff7684ff68d5ff8272ff72d0ff72f8");
    auto actual = utf8_to_rct2(input);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, UTF8_to_RCT2_PL)
{
    auto input = u8"Górska Dołki Tęczowy";
    auto expected = StringFromHex("47F372736b6120446ff76b692054e6637a6f7779");
    auto actual = utf8_to_rct2(input);
    ASSERT_EQ(expected, actual);
}
