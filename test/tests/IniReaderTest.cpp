/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2/config/IniReader.hpp"

#include "openrct2/config/ConfigEnum.hpp"
#include "openrct2/core/MemoryStream.h"

#include <gtest/gtest.h>
#include <limits>
#include <string>

class IniReaderTest : public testing::Test
{
protected:
    static const std::string predefined;
    static const std::string duplicate;
    static const std::string untrimmed;
    static const std::string caseInsensitive;
};

static auto Enum_Currency = ConfigEnum<int32_t>({});

TEST_F(IniReaderTest, create_empty)
{
    OpenRCT2::MemoryStream ms(0);
    ASSERT_EQ(ms.CanRead(), true);
    ASSERT_EQ(ms.CanWrite(), true);
    auto ir = CreateIniReader(&ms);
    ASSERT_NE(ir, nullptr);
    ASSERT_EQ(ir->GetBoolean("nobody", true), true);
    ASSERT_EQ(ir->GetCString("expects", nullptr), nullptr);
    ASSERT_EQ(ir->GetEnum<int32_t>("spanish", 12345, Enum_Currency), 12345);
    ASSERT_EQ(ir->GetFloat("inquisition", 1.234f), 1.234f);
    ASSERT_EQ(ir->GetInt32("universal_answer", 42), 42);
    ASSERT_EQ(
        ir->GetInt64("heat_death_of_the_universe", std::numeric_limits<int64_t>::max()), std::numeric_limits<int64_t>::max());
}

TEST_F(IniReaderTest, read_prepared)
{
    OpenRCT2::MemoryStream ms(predefined.c_str(), predefined.size());
    ASSERT_EQ(ms.CanRead(), true);
    ASSERT_EQ(ms.CanWrite(), false);
    auto ir = CreateIniReader(&ms);
    ASSERT_NE(ir, nullptr);
    ASSERT_EQ(ir->ReadSection("doesnt_exist"), false);
    ASSERT_EQ(ir->ReadSection("bool"), true);
    // name of section
    ASSERT_EQ(ir->GetInt32("bool", 42), 42);
    // value from different section
    ASSERT_EQ(ir->GetInt32("one", 42), 42);
    // existing value as different type
    ASSERT_EQ(ir->GetInt32("boolval", 42), 42);
    ASSERT_EQ(ir->GetBoolean("boolval", false), true);
    // skip one section
    ASSERT_EQ(ir->ReadSection("string"), true);
    // values from different sections
    ASSERT_EQ(ir->GetInt32("one", 42), 42);
    ASSERT_EQ(ir->GetBoolean("boolval", false), true);
    const utf8* str = ir->GetCString("path", nullptr);
    ASSERT_STREQ(str, u8"C:'\\some/dir\\here/神鷹暢遊");
    Memory::Free(str);
    // go back a section
    ASSERT_EQ(ir->ReadSection("int"), true);
    ASSERT_EQ(ir->GetInt32("one", 42), 1);
}

TEST_F(IniReaderTest, read_duplicate)
{
    OpenRCT2::MemoryStream ms(duplicate.c_str(), duplicate.size());
    ASSERT_EQ(ms.CanRead(), true);
    ASSERT_EQ(ms.CanWrite(), false);
    auto ir = CreateIniReader(&ms);
    ASSERT_NE(ir, nullptr);
    // there should only be data from the last section
    ASSERT_EQ(ir->ReadSection("section"), true);
    ASSERT_EQ(ir->GetBoolean("one", false), false);
    ASSERT_EQ(ir->GetBoolean("two", false), false);
    ASSERT_EQ(ir->GetBoolean("three", false), true);
    ASSERT_EQ(ir->ReadSection("section"), true);
    // try switching to another section
    ASSERT_EQ(ir->ReadSection("doesnt_exist"), false);
    // make sure we are still in the same section
    ASSERT_EQ(ir->GetBoolean("one", false), false);
    ASSERT_EQ(ir->GetBoolean("two", false), false);
    ASSERT_EQ(ir->GetBoolean("three", false), true);
    ASSERT_EQ(ir->GetInt32("fortytwo", 100), 41);
    ASSERT_EQ(ir->ReadSection("section"), true);
    // test 4 times, there are only 3 sections
    ASSERT_EQ(ir->ReadSection("section"), true);
}

TEST_F(IniReaderTest, read_untrimmed)
{
    OpenRCT2::MemoryStream ms(untrimmed.c_str(), untrimmed.size());
    ASSERT_EQ(ms.CanRead(), true);
    ASSERT_EQ(ms.CanWrite(), false);
    auto ir = CreateIniReader(&ms);
    ASSERT_NE(ir, nullptr);
    // there should only be data from the last section
    ASSERT_EQ(ir->ReadSection("section"), true);
    ASSERT_EQ(ir->GetBoolean("one", false), true);
    const utf8* str = ir->GetCString("str", nullptr);
    ASSERT_STREQ(str, "  xxx ");
    Memory::Free(str);
    ASSERT_EQ(ir->GetString("str", "yyy"), "  xxx ");
    ASSERT_EQ(ir->GetString("nosuchthing", "  yyy "), "  yyy ");
}

TEST_F(IniReaderTest, read_case_insensitive)
{
    OpenRCT2::MemoryStream ms(caseInsensitive.c_str(), caseInsensitive.size());
    ASSERT_EQ(ms.CanRead(), true);
    ASSERT_EQ(ms.CanWrite(), false);
    auto ir = CreateIniReader(&ms);
    ASSERT_NE(ir, nullptr);
    ASSERT_EQ(ir->ReadSection("section"), true);
    ASSERT_EQ(ir->GetString("foo", "yyy"), "bar");
    ASSERT_EQ(ir->ReadSection("SeCtIoN"), true);
}

const std::string IniReaderTest::predefined = "[bool]\n"
                                              "boolval = true\n\n"
                                              "[int]\n"
                                              "one = 1\n"
                                              "zero = 0\n\n"
                                              "[string]\n"
                                              "path = "
                                              "\"C:'\\\\some/dir\\\\here/\xE7\xA5\x9E\xE9\xB7\xB9\xE6\x9A\xA2\xE9\x81\x8A\"\n";

const std::string IniReaderTest::duplicate = "[section]\n"
                                             "one = true\n"
                                             "fortytwo = 13\n"
                                             "[section]\n"
                                             "two = true\n"
                                             "[section]\n"
                                             "three = true\n"
                                             "fortytwo = 42\n"
                                             "fortytwo = 41\n";

const std::string IniReaderTest::untrimmed = "[section]\n"
                                             "one =     true      \n"
                                             "    str =    \"  xxx \"";

const std::string IniReaderTest::caseInsensitive = "[sEcTiOn]\n"
                                                   "foo = \"bar\"\n";
