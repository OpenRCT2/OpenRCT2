/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2/localisation/Localisation.h"

#include "helpers/StringHelpers.hpp"

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
    auto input = StringFromHex("ff8374ff8340ff8358ff8367ff8375ff8389ff8345ff8393ff8374ff8348ff8362ff834eff8358");
    auto expected = u8"ファストブラウンフォックス";
    auto actual = rct2_to_utf8(input, RCT2_LANGUAGE_ID_JAPANESE);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, RCT2_to_UTF8_ZH_TW)
{
    auto input = StringFromHex("ffa7d6ffb374ffaabaffb4c4ffa6e2ffaab0ffaf57");
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

TEST_F(Localisation, RCT2_to_UTF8_ZH_TW_PREMATURE_END)
{
    // This string can be found in BATFL.DAT, the last double byte character is missing its second byte.
    auto input = StringFromHex("ffa470ffabacffa8aeffbdf8ffa662ffc54bffb944ffa457ffaeb6ffb0caffb76effc2");
    auto expected = u8"小型車輛在鐵道上振動搖";
    auto actual = rct2_to_utf8(input, RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL);
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
