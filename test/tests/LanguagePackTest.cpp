/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2/localisation/LanguagePack.h"

#include "openrct2/localisation/Language.h"
#include "openrct2/localisation/StringIds.h"

#include <gtest/gtest.h>

using namespace OpenRCT2;

class LanguagePackTest : public testing::Test
{
protected:
    static const utf8* LanguageEnGB;
    static const utf8* LanguageZhTW;
};

TEST_F(LanguagePackTest, create_empty)
{
    auto empty = LanguagePackFactory::FromText(0, "");
    ASSERT_EQ(empty->GetId(), 0);
    ASSERT_EQ(empty->GetCount(), 0u);
}

TEST_F(LanguagePackTest, create_mutable_id_1)
{
    auto lang = LanguagePackFactory::FromText(1, "STR_0000:\n");
    ASSERT_EQ(lang->GetId(), 1);
    ASSERT_EQ(lang->GetCount(), 1u);
    ASSERT_STREQ(lang->GetString(0), nullptr);
    lang->SetString(0, "xx");
    ASSERT_EQ(lang->GetCount(), 1u);
    ASSERT_STREQ(lang->GetString(0), "xx");
}

TEST_F(LanguagePackTest, language_pack_simple)
{
    auto lang = LanguagePackFactory::FromText(0, LanguageEnGB);
    ASSERT_EQ(lang->GetId(), 0);
    ASSERT_EQ(lang->GetCount(), 4u);
    ASSERT_STREQ(lang->GetString(2), "Spiral Roller Coaster");
    // Test some negatives too
    ASSERT_EQ(lang->GetString(1000), nullptr);
}

TEST_F(LanguagePackTest, language_pack_multibyte)
{
    auto lang = LanguagePackFactory::FromText(0, LanguageZhTW);
    ASSERT_EQ(lang->GetId(), 0);
    ASSERT_EQ(lang->GetCount(), 4u);
    ASSERT_STREQ(lang->GetString(2), u8"懸吊式雲霄飛車");
}

const utf8* LanguagePackTest::LanguageEnGB = "# STR_XXXX part is read and XXXX becomes the string id number.\n"
                                             "# Everything after the colon and before the new line will be saved as the "
                                             "string.\n"
                                             "# Use # at the beginning of a line to leave a comment.\n"
                                             "STR_0000    :\n"
                                             "STR_0001    :{STRINGID} {COMMA16}\n"
                                             "STR_0002    :Spiral Roller Coaster\n"
                                             "STR_0003    :Stand-up Roller Coaster\n";

// This includes a few entries extracted from zh-TW localisation.
const utf8* LanguagePackTest::LanguageZhTW = u8"STR_0000    :螺旋式雲霄飛車\n"
                                             u8"STR_0001    :站立式雲霄飛車\n"
                                             u8"STR_0002    :懸吊式雲霄飛車\n"
                                             u8"STR_0003    :反轉式雲霄飛車\n";
