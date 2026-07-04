/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "helpers/StringHelpers.hpp"
#include "openrct2/localisation/Language.h"
#include "openrct2/rct12/CSStringConverter.h"

#include <gtest/gtest.h>

using namespace OpenRCT2;

class Localisation : public testing::Test
{
};

///////////////////////////////////////////////////////////////////////////////
// Tests for RCT2StringToUTF8
///////////////////////////////////////////////////////////////////////////////

TEST_F(Localisation, RCT2_to_UTF8_UK)
{
    auto input = "The quick brown fox";
    auto expected = u8"The quick brown fox";
    auto actual = RCT2StringToUTF8(input, RCT2LanguageId::englishUK);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, RCT2_to_UTF8_JP)
{
    auto input = StringFromHex("ff8374ff8340ff8358ff8367ff8375ff8389ff8345ff8393ff8374ff8348ff8362ff834eff8358");
    auto expected = u8"ファストブラウンフォックス";
    auto actual = RCT2StringToUTF8(input, RCT2LanguageId::japanese);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, RCT2_to_UTF8_ZH_TW)
{
    auto input = StringFromHex("ffa7d6ffb374ffaabaffb4c4ffa6e2ffaab0ffaf57");
    auto expected = u8"快速的棕色狐狸";
    auto actual = RCT2StringToUTF8(input, RCT2LanguageId::chineseTraditional);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, RCT2_to_UTF8_PL)
{
    auto input = StringFromHex("47F372736b6120446ff76b692054e6637a6f7779");
    auto expected = u8"Górska Dołki Tęczowy";
    auto actual = RCT2StringToUTF8(input, RCT2LanguageId::englishUK);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, RCT2_to_UTF8_ZH_TW_PREMATURE_END)
{
    // This string can be found in BATFL.DAT, the last double byte character is missing its second byte.
    auto input = StringFromHex("ffa470ffabacffa8aeffbdf8ffa662ffc54bffb944ffa457ffaeb6ffb0caffb76effc2");
    auto expected = u8"小型車輛在鐵道上振動搖";
    auto actual = RCT2StringToUTF8(input, RCT2LanguageId::chineseTraditional);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, RCT2_to_UTF8_ALREADY_UTF8)
{
    // Scenario names/details written by OpenRCT2 itself are stored as UTF-8; they must pass
    // through unchanged even though the importer requests the englishUK conversion.
    auto input = StringFromHex("ebaaa8eb9190ec9d9820ed8380ec9db4ecbfa4");
    auto expected = u8"모두의 타이쿤";
    auto actual = RCT2StringToUTF8(input, RCT2LanguageId::englishUK);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, RCT2_to_UTF8_KO_RAW_CP949)
{
    // Parks made with the Korean release of RCT2 store raw CP949 byte pairs, but the importer
    // hardcodes englishUK; the CP949 hangul zone must be auto-detected.
    auto input = StringFromHex("b8f0b5cec0c720c5b8c0ccc4ef");
    auto expected = u8"모두의 타이쿤";
    auto actual = RCT2StringToUTF8(input, RCT2LanguageId::englishUK);
    ASSERT_EQ(expected, actual);
}

TEST_F(Localisation, RCT2_to_UTF8_KO_WIDECHAR_ESCAPED)
{
    // User strings (park/ride names) written by OpenRCT2 encode non-ASCII as 0xFF-escaped
    // UTF-16 code units; the table path passes those through and CP949 detection must not fire.
    auto input = StringFromHex("ffbaa8ffb450ffc75820ffd0c0ffc774ffcfe4");
    auto expected = u8"모두의 타이쿤";
    auto actual = RCT2StringToUTF8(input, RCT2LanguageId::englishUK);
    ASSERT_EQ(expected, actual);
}
